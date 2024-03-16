#ifndef HASHLIB_H_
#define HASHLIB_H_

#include "types.h"

uint8_t inline xor_hash(uint8_t bhr, uint8_t pc_index) {
    return bhr ^ pc_index;
}

/* adopting the data from libcrc */
/* including the MIT License to comply requirement */
/*
 * Library: libcrc
 * File:    src/crc8.c
 * Author:  Lammert Bies
 *
 * This file is licensed under the MIT License as stated below
 *
 * Copyright (c) 1999-2016 Lammert Bies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Description
 * -----------
 * The source file src/crc8.c contains routines for the calculation of 8 bit
 * CRC values according to the calculation rules used in the SHT1x and SHT7x
 * series of temperature and humidity sensors.
 */
static uint8_t crc_table[] = { /* BEGIN ADOPTION */
0,   49,  98,  83,  196, 245, 166, 151, 185, 136, 219, 234, 125, 76,  31,  46,
67,  114, 33,  16,  135, 182, 229, 212, 250, 203, 152, 169, 62,  15,  92,  109,
134, 183, 228, 213, 66,  115, 32,  17,  63,  14,  93,  108, 251, 202, 153, 168,
197, 244, 167, 150, 1,   48,  99,  82,  124, 77,  30,  47,  184, 137, 218, 235,
61,  12,  95,  110, 249, 200, 155, 170, 132, 181, 230, 215, 64,  113, 34,  19,
126, 79,  28,  45,  186, 139, 216, 233, 199, 246, 165, 148, 3,   50,  97,  80,
187, 138, 217, 232, 127, 78,  29,  44,  2,   51,  96,  81,  198, 247, 164, 149,
248, 201, 154, 171, 60,  13,  94,  111, 65,  112, 35,  18,  133, 180, 231, 214,
122, 75,  24,  41,  190, 143, 220, 237, 195, 242, 161, 144, 7,   54,  101, 84,
57,  8,   91,  106, 253, 204, 159, 174, 128, 177, 226, 211, 68,  117, 38,  23,
252, 205, 158, 175, 56,  9,   90,  107, 69,  116, 39,  22,  129, 176, 227, 210,
191, 142, 221, 236, 123, 74,  25,  40,  6,   55,  100, 85,  194, 243, 160, 145,
71,  118, 37,  20,  131, 178, 225, 208, 254, 207, 156, 173, 58,  11,  88,  105,
4,   53,  102, 87,  192, 241, 162, 147, 189, 140, 223, 238, 121, 72,  27,  42,
193, 240, 163, 146, 5,   52,  103, 86,  120, 73,  26,  43,  188, 141, 222, 239,
130, 179, 224, 209, 70,  119, 36,  21,  59,  10,  89,  104, 255, 206, 157, 172
}; /* END ADOPTION */

uint8_t inline crc_hash(uint8_t bhr, uint8_t pc_index) {
    return crc_table[pc_index ^ crc_table[bhr]];
}

#define HASH_FUNC crc_hash
#define HASH_FUNC_NAME "CRC"

/*
Original Benchmark Data:

----- dhrystone -----
PERF: instrs=230174, cycles=2716722, bpred=88%, hashtime=68.613
PERF: instrs=230203, cycles=2717502, bpred=87%, hashtime=65.3458
----- median -----
PERF: instrs=12152, cycles=101114, bpred=74%, hashtime=64.4286
PERF: instrs=12152, cycles=101203, bpred=74%, hashtime=61.2913
----- memcpy -----
PERF: instrs=38158, cycles=367481, bpred=92%, hashtime=69.4273
PERF: instrs=38158, cycles=367442, bpred=92%, hashtime=67.259
----- multiply -----
PERF: instrs=28002, cycles=81962, bpred=82%, hashtime=67.0102
PERF: instrs=28011, cycles=82019, bpred=81%, hashtime=61.7083
----- qsort -----
PERF: instrs=144162, cycles=2756249, bpred=70%, hashtime=74.2556
PERF: instrs=144162, cycles=2758151, bpred=66%, hashtime=68.3194
----- rsort -----
PERF: instrs=191917, cycles=2559653, bpred=90%, hashtime=65.5773
PERF: instrs=191917, cycles=2559605, bpred=91%, hashtime=61.3783
----- spmv -----
PERF: instrs=1982861, cycles=5270298, bpred=77%, hashtime=66.6892
PERF: instrs=1982834, cycles=5265771, bpred=76%, hashtime=65.0342
----- towers -----
PERF: instrs=10519, cycles=104157, bpred=65%, hashtime=71.237
PERF: instrs=10519, cycles=104184, bpred=64%, hashtime=63.6839

*/

#endif