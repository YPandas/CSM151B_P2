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

#pragma once
#include "types.h"
#include "hashlib.h"
#define BTB_VALID_BIT(x) (((x)>>22)&1)
#define BTB_TAG(x) ((x)&0x3fffff) 
#define EXTRACT_BTB_META(meta, valid, tag) uint8_t valid=(BTB_VALID_BIT(meta));uint32_t tag=(BTB_TAG(meta))
#define BUILD_BTB_META(tag, valid) ((((valid)&1)<<22)|((tag)&0x3fffff))
// crop lower 22bit: 0x3fffff = 0b1111111111111111111111
namespace tinyrv
{

    struct pipeline_trace_t;

    class GShare
    {
    public:
        GShare();

        ~GShare();

        bool predict(pipeline_trace_t *trace);

        unsigned long long inline getTotalHashTime() {return total_hash_time;}

    private:
        uint8_t bhr = 0x0; // 8-bit BHR
        uint8_t bht[256];
        Word btb_target[256];
        uint32_t btb_meta[256]; // 32-8-2 = 22 bit tag; 1 valid bit
        unsigned long long total_hash_time;
    };

}