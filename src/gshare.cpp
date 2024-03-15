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
#include <cstdio>

using namespace tinyrv;

GShare::GShare()
{
    //--
    bhr = 0x0; // 8-bit BHR
    int broom = 256;
    while (broom --> 0) // sweep it!
    {
        bht[broom] = 0x0;
        btb_target[broom] = -1;
        btb_meta[broom] = 0;
    }
}

GShare::~GShare()
{
    //--
}

bool GShare::predict(pipeline_trace_t *trace)
{
    uint8_t pc_index = (trace->PC >> 2) & 0xFF; // 30-bit PC
    uint32_t pc_tag = (trace->PC >> 2) >> 8; // adopting slides.
    // strictly following the steps...
    unsigned int index = (this->bhr ^ pc_index) & 0xFF;
    Word predicted_next_pc = this->btb_target[pc_index];
    // evil meta-progamming
    EXTRACT_BTB_META(this->btb_meta[pc_index], valid, tag);
    if(!valid || tag != pc_tag) predicted_next_pc = -1;

    bool predicted_taken = (this->bht[index] >= 2);
    // debug print
    // printf("BHR = 0x%x, pc_byte = 0x%x, index = %u, this->bht[index] = 0x%x\n", this->bhr, pc_byte, index, this->bht[index]);
    DP(3, "*** GShare: "
              << "BHR=0x" << std::hex << (int)this->bhr << ", PHT_index="
              << index << ", PHT_taken=" << predicted_taken << ", BTB_nextPC=0x"
              << predicted_next_pc << ": " << *trace);
    // determine if the prediction is correct
    bool ans = (predicted_taken && ((predicted_next_pc << 2) == trace->next_pc)) ||
               (!predicted_taken && !trace->actual_taken);
    // another debug log
    DP(3, "*** GShare: predicted " << (predicted_taken ? "taken" : "not-taken")
                                   << "=" << ans << ": " << *trace);

    // update with actual result (hacking weak typing)
    if (trace->actual_taken) {
        this->btb_target[pc_index] = ((trace->next_pc) >> 2);
        this->btb_meta[pc_index] = BUILD_BTB_META(pc_tag, 1);
    }

    if (trace->actual_taken)
    {
        if (this->bht[index] + 1 < 4)
            this->bht[index]++;
    }
    else
    {
        if (this->bht[index] != 0)
            this->bht[index]--;
    }
    this->bhr = ((this->bhr << 1) | (trace->actual_taken ? 1 : 0)) & 0xFF;
    return ans;
}