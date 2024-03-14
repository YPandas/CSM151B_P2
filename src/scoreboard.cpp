// Copyright 2024 blaise
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <assert.h>
#include <util.h>
#include "types.h"
#include "scoreboard.h"
#include "core.h"
#include "debug.h"

using namespace tinyrv;

Scoreboard::Scoreboard(Core* core, uint32_t num_RSs, uint32_t rob_size) 
  : core_(core)  
  , RAT_(NUM_REGS)
  , RS_(num_RSs)
  , RST_(rob_size, -1) {
  // create the ROB
  ROB_ = ReorderBuffer::Create(this, ROB_SIZE);
}

Scoreboard::~Scoreboard() {
  //--
}

bool Scoreboard::issue(pipeline_trace_t* trace) {
  auto& ROB = ROB_;
  auto& RAT = RAT_;
  auto& core = core_;
  
  // TODO:
  // check for structural hazards return false if found
  FUType requiredType = trace->fu_type;
  auto& fu = core->FUs_[(int)requiredType];
  if(fu->isBusy){
    //if the function unit is busy
    return false;
  } else {
    //if the unit is free
    fu->isBusy = true;
  }

  // TODO:
  // load renamed operands (rob1_index, rob2_index) from RAT
  // use the instruction source operands from trace
  // a returned value of-1 indicate that the register value is in the register file, otherwise it is in the RS or ROB)
  int rob1_index = RAT.get(trace->rs1); 
  int rob2_index = RAT.get(trace->rs2); 
     
  // for each non-available operands (value == -1), obtain their producing RS indices (rs1_index, rs2_index) from the RST
  // setting rs_index=-1 means the operand value is in the ROB or register File.
  int rs1_index = (rob1_index != -1) ? RST_[rob1_index] : -1;
  int rs2_index = (rob2_index != -1) ? RST_[rob2_index] : -1;

  // allocate new ROB entry
  int rob_index = ROB->allocate(trace);

  // update the RAT if instruction is writing to the register file
  if (trace->wb) {
    RAT.set(trace->rd, rob_index);
  }

  // push trace to RS and obtain index
  int rs_index = RS_.push(trace, rob_index, rs1_index, rs2_index);

  // update the RST with newly allocated RS index
  RST_[rob_index] = rs_index;

  return true;
}

std::vector<pipeline_trace_t*> Scoreboard::execute() {
  std::vector<pipeline_trace_t*> traces;
  auto& FUs = core_->FUs_;

  // TODO:
  // search the RS for any valid and not yet running entry
  // that is ready (i.e. both rs1_index and rs2_index are -1)
  // send it to its corresponding FUs
  // mark it as running
  // add its trace to return list
  for (int i = 0; i < (int)RS_.size(); ++i) { 
    auto& rs_entry = RS_[i];
    // HERE!
    if(rs_entry.rs1_index == -1 && rs_entry.rs2_index == -1){
      //if both rs are ready
      FUType requiredType = rs_entry.trace->fu_type;
      auto& fu = core_->FUs_[(int)requiredType];
      auto trace = rs_entry.trace;
      fu->Input.send({trace, 0, 0});
      rs_entry.running = true;
      traces.push_back(trace);
    }
  }

  return traces;
}

pipeline_trace_t* Scoreboard::writeback() {
  pipeline_trace_t* trace = nullptr;
  auto& ROB = ROB_;
  auto& FUs = core_->FUs_;

  // process the first FU to have completed execution by accessing its output
  for (auto& fu : FUs) {
    if (fu->Output.empty())
      continue;

    auto& fu_entry = fu->Output.front();

    // TODO:
    // broadcast result to all RS pending for this FU's rs_index
    // invalidate matching rs_index by setting it to -1 to imply that the operand value is now available
    for (uint32_t i = 0; i < RS_.size(); ++i) { 
      auto& rs_entry = RS_[i];
      if (!rs_entry.valid)
        continue;
        
      // HERE!
      if(rs_entry.rs1_index == fu_entry.rob_index){
        //operand value is now available
        rs_entry.rs1_index = -1;
      }
      if(rs_entry.rs2_index == fu_entry.rob_index){
        //operand value is now available
        rs_entry.rs2_index = -1;
      }
    }
    
    // TODO: 
    // clear RST by invalidating current ROB entry to -1
    RST_[fu_entry.rob_index].valid = false;
        
    // TODO: 
    // notify the ROB about completion (using ROB->Completed.send())
    ROB->Completed.send(fu_entry.trace);
    // TODO: 
    // deallocate the RS entry of this FU
    RS_[fu_entry.rs_index].valid = false;
    // set the returned trace
    trace = fu_entry.trace;

    // remove FU entry
    fu->Output.pop();

    // we process one FU at the time
    break;
  }

  return trace;
}

pipeline_trace_t* Scoreboard::commit() {
  pipeline_trace_t* trace = nullptr;
  if (!ROB_->Committed.empty()) {
    trace = ROB_->Committed.front();
    ROB_->Committed.pop();
  }
  return trace;
}

void Scoreboard::dump() {
  RS_.dump();
  ROB_->dump();
}