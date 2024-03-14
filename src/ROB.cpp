// Copyright 2024 Blaise Tine
// 
// Licensed under the Apache License;
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
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
#include "debug.h"
#include "ROB.h"

using namespace tinyrv;

ReorderBuffer::ReorderBuffer(const SimContext& ctx, Scoreboard* scoreboard, uint32_t size) 
  : SimObject<ReorderBuffer>(ctx, "ReorderBuffer")
  , Completed(this)
  , Committed(this)
  , scoreboard_(scoreboard)
  , store_(size) {
  this->reset();
}

ReorderBuffer::~ReorderBuffer() {
  //--
}

void ReorderBuffer::reset() {
  for (auto& entry : store_) {
    entry.trace = nullptr;
    entry.completed = false;
  }
  head_index_ = 0;
  tail_index_ = 0;
  count_ = 0;
}

void ReorderBuffer::tick() {
  if (this->is_empty())
    return;

  auto& RAT = scoreboard_->RAT_;
  
  // check if we have a completed instruction
  if (!Completed.empty()) {
    // mark its entry as completed
    int rob_index = Completed.front();
    store_[rob_index].completed = true;
    Completed.pop();
  }

  // get the head entry
  auto& head = store_[head_index_];
  
  // TODO:
  // check if head entry has completed
  // clear the RAT if it is still pointing to this ROB entry
  // use the destination register from trace to access the RAT    
  // Warning: only update the RAT for instructions that write to the register file 
  // push the trace into commit port (using this->Committed.send())
  // remove the head entry
  // HERE!
}

int ReorderBuffer::allocate(pipeline_trace_t* trace) {
  assert(!this->is_full());
  if (this->is_full())
    return -1;  
  int index = tail_index_;
  store_[index] = {trace, false};
  tail_index_ = (tail_index_ + 1) % store_.size();
  ++count_;  
  return index;
}

int ReorderBuffer::pop() {
  assert(!this->is_empty());
  assert(store_[head_index_].trace != nullptr);
  assert(store_[head_index_].completed);
  if (is_empty())
    return -1;
  store_[head_index_].trace = nullptr;
  store_[head_index_].completed = false;
  head_index_ = (head_index_ + 1) % store_.size();
  --count_;
  return head_index_;
}

bool ReorderBuffer::is_full() const {
  return count_ == store_.size();
}

bool ReorderBuffer::is_empty() const {
  return count_ == 0;
}

void ReorderBuffer::dump() {
  for (int i = 0; i < (int)store_.size(); ++i) {
    auto& entry = store_[i];
    if (entry.trace != nullptr) {
      DT(4, "ROB[" << i << "] completed=" << entry.completed << ", head=" << (i == head_index_) << ", trace=" << *entry.trace);
    }
  }
}