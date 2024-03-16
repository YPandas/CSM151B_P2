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

  for (const auto& value : RST_) {
    DP(4, "RST state = " << value << " ");
  }

  
  
  // TODO:
  // check for structural hazards return false if found
  FUType requiredType = trace->fu_type;

  if(ROB->is_full()){
    //debug print
    DP(4, "ROB is busy");
    core->branch_stalls_ = 1;
    return false;
  } else {
    core->branch_stalls_ = 0;
  }

  if(RS_.is_full()){
    //DP(4, "the RS is full");
    core->branch_stalls_ = 1;
    return false;
  }

  // TODO:
  // load renamed operands (rob1_index, rob2_index) from RAT
  // use the instruction source operands from trace
  // a returned value of-1 indicate that the register value is in the register file, otherwise it is in the RS or ROB)
  int rob1_index = RAT.get(trace->rs1); 
  DP(4, "the actual rs1 = " << trace->rs1);
  int rob2_index = RAT.get(trace->rs2); 
  DP(4, "the actual rs2 = " << trace->rs2);
     
  // for each non-available operands (value == -1), obtain their producing RS indices (rs1_index, rs2_index) from the RST
  // setting rs_index=-1 means the operand value is in the ROB or register File.
  int rs1_index = (rob1_index != -1) ? RST_[rob1_index] : -1;
  int rs2_index = (rob2_index != -1) ? RST_[rob2_index] : -1;

  // allocate new ROB entry
  int rob_index = ROB->allocate(trace);

  if(rs1_index != -1){
    DP(4, "rs 1 index is not -1, it's " << rs1_index << " it's rob = " << rob_index << " the needed rs = " << trace->rs1);
  }
  if(rs2_index != -1){
    DP(4, "rs 2 index is not -1, it's " << rs2_index << " it's rob = " << rob_index << " the needed rs = " << trace->rs2);
  }

  // update the RAT if instruction is writing to the register file
  if (trace->wb) {
    RAT.set(trace->rd, rob_index);
  }

  // push trace to RS and obtain index
  int rs_index = RS_.push(trace, rob_index, rs1_index, rs2_index);
  DP(4, "pushed entry rs index = " << rs_index
  << " rob_index = " << rob_index << " rs1 = " << rs1_index 
  << " rs2 = " << rs2_index);

  // update the RST with newly allocated RS index
  DP(4, "UPDATE RST["<< rob_index <<"]:="<<rs_index<<", trace:"<< *trace <<std::endl);
  RST_[rob_index] = rs_index;
  return true;
}

std::vector<pipeline_trace_t*> Scoreboard::execute() {
  DP(4, "start to execute");
  std::vector<pipeline_trace_t*> traces;

  // TODO:
  // search the RS for any valid and not yet running entry
  // that is ready (i.e. both rs1_index and rs2_index are -1)
  // send it to its corresponding FUs
  // mark it as running
  // add its trace to return list
  for (int i = 0; i < (int)RS_.size(); ++i) { 
    auto& rs_entry = RS_[i];
    // HERE!
    if(rs_entry.rs1_index != -1 || rs_entry.rs2_index != -1){
      DP(4, "rob_index = " << rs_entry.rob_index << " entry not ready");
      DP(4, "rs1 index = " << rs_entry.rs1_index << " rs2 index = " << rs_entry.rs2_index);
    }
    DP(4, "currently checking rs = " << i);

    if(rs_entry.rs1_index == -1 && rs_entry.rs2_index == -1 && !rs_entry.running && rs_entry.valid){
      //if both rs are ready and rs_entry hasn't started running yet
      FUType requiredType = rs_entry.trace->fu_type;
      DP(4, "required Type = " << (int)requiredType);
      auto& fu = core_->FUs_[(int)requiredType];
      if ( !core_->FUs_[(int)requiredType]) {
        DP(4, "Invalid FU reference for type: " << requiredType);
      }
      auto trace = rs_entry.trace;
      DP(4, "trace rd = " << trace->rd);
      FunctionalUnit::entry_t packet;
      packet.trace = trace;
      packet.rob_index = rs_entry.rob_index;
      packet.rs_index = i;
      DP(4, "Before accessing Input");
      if (fu->isBusy) {
        DP(4, "fu is busy");
      } else {
        DP(4, "fu is free");
      }
      DP(4, "sending packet rs_index = " << i);
      fu->Input.send(packet);
      rs_entry.running = true;
      //DP(4, "trace pushing");

      traces.push_back(trace);
    }
  }
  DP(4, "trace returning...");
  return traces;
}

pipeline_trace_t* Scoreboard::writeback() {
  DP(4, "start to write back");
  pipeline_trace_t* trace = nullptr;
  auto& ROB = ROB_;
  auto& FUs = core_->FUs_;
  // process the first FU to have completed execution by accessing its output
  for (auto& fu : FUs) {
    if (fu->Output.empty()){
      DP(4, "fu output is empty");
      continue;
    }
    DP(4, "accesing output...");
    if (!fu->Output.connected()) {
        DP(4, "Output port is not connected.");

    } else {
        DP(4, "Output port is connected.");
    }
    auto& fu_entry = fu->Output.front();
    DP(4, "done accessing output");

    // TODO:
    // broadcast result to all RS pending for this FU's rs_index
    // invalidate matching rs_index by setting it to -1 to imply that the operand value is now available
    for (uint32_t i = 0; i < RS_.size(); ++i) { 
      auto& rs_entry = RS_[i];
      DP(4, "RS entry " << i << "is " << (rs_entry.valid ? "valid" : "not valid"));
      if (!rs_entry.valid)
        continue;
        
      // HERE!
      DP(4, "RELEASE ROB :"
            << ", FU_PC=0x" << std::hex << fu_entry.trace->PC
            << ", RS_PC=0x" << rs_entry.trace->PC
            << ", RS1_INDEX="<<std::dec << rs_entry.rs1_index
            << ", RS2_INDEX="<<rs_entry.rs2_index
            << ", ROB_INDEX="<<fu_entry.rob_index);
      if(rs_entry.rs1_index == RST_[fu_entry.rob_index]){
        //operand value is now available
        DP(4, "updated rs1 index = " << rs_entry.rs1_index);
        rs_entry.rs1_index = -1;
      }
      if(rs_entry.rs2_index == RST_[fu_entry.rob_index]){
        //operand value is now available
        DP(4, "updated rs2 index = " << rs_entry.rs2_index);
        rs_entry.rs2_index = -1;
      }
    }
    
    // TODO: 
    // clear RST by invalidating current ROB entry to -1
    RST_[fu_entry.rob_index] = -1;
        
    // TODO: 
    // notify the ROB about completion (using ROB->Completed.send())
    ROB->Completed.send(fu_entry.rob_index);
    // TODO: 
    // deallocate the RS entry of this FU
    DP(4, "Removing RS entry at index: " << fu_entry.rs_index);
    RS_.remove(fu_entry.rs_index);
    // set the returned trace
    trace = fu_entry.trace;

    // remove FU entry
    fu->Output.pop();
    fu->isBusy = false;
  
    // we process one FU at the time
    break;
  }

  return trace;
}

pipeline_trace_t* Scoreboard::commit() {
  pipeline_trace_t* trace = nullptr;
  if (!ROB_->Committed.empty()) {
    
    trace = ROB_->Committed.front();
    DP(4, "commiting: "<<*trace);
    ROB_->Committed.pop();
  }
  return trace;
}

void Scoreboard::dump() {
  RS_.dump();
  ROB_->dump();
}