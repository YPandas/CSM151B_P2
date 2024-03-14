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
#include "core.h"
#include "debug.h"
#include "trace.h"

using namespace tinyrv;

GShare::GShare()
{
    //--
    bhr = 0x0; // 8-bit BHR
    int broom = 0xFF;
    while(broom --> 0) btb[broom] = bht[broom] = 0x0;
    
}

GShare::~GShare()
{
    //--
}

bool GShare::predict(pipeline_trace_t *trace)
{
    // strictly following the steps...
    Word predicted_next_pc = this->btb[trace->PC & 0xFF];
    unsigned int index = this->bhr ^ (trace->PC & 0xFF);
    bool predicted_taken = (this->bht[index] < 2);
    // determine if the prediction is correct
    bool ans = (predicted_taken && predicted_next_pc == trace->next_pc) ||
               (!predicted_taken && !trace->actual_taken);
    // update with actual result (hacking weak typing)
    if (trace->actual_taken)
        this->btb[trace->PC & 0xFF] = trace->next_pc;
    this->bht[index] = this->TSFR_TAB[this->bht[index]][trace->actual_taken];
    this->bhr = (this->bhr << 1) & trace->actual_taken;
    return ans;
}