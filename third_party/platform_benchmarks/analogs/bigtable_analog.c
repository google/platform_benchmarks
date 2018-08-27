/*
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "third_party/platform_benchmarks/result.h"
#include "third_party/platform_benchmarks/util.h"

struct Result bigtable_analog() {
  struct Result result;
  uint64_t* m = (uint64_t *)malloc(sizeof(uint64_t) * LOOP2M);
  uint64_t accum = 0;


  uint64_t t = now_nsec();
  register int i;
  for (i = 0; i < LOOP4K; i++) {
    uint64_t ptr = (uint64_t)m;

    // Per iteration of inner macro
    // ... 7 instr-bytes per mem access and increment
    // ... 8 byte data access
    // Per iteration of outer macro...
    //   Code bytes
    // .... 16 x 3 x (7-byte) = 336 bytes
    // .... 2 x (jmp 1f, nop) = 2 x (5 + 1) = 12 bytes
    // .... test, jnz, nop = 3 + 6 + 1 = 10 bytes
    // .... TOTAL = 358 byte
    // .... Instructions = 16 x 3 x 2 + 2 x 2 + 3 = 103
    // .... Branches = 2 + 1 = 3 = ~30 pKI
    //   Data bytes
    // .... 16 x 3 x 8 = 384 bytes
    // TOTAL
    // Code = 32768 x 358 = 11.19 MB
    // Instruction = 32768 x 103 = 3.375 million
    // Branches = 32768 x 3 = 96k (all likely miss BTB)
    //          = ~30 pKI
    // Data = 32768 x 384 = 12 MB

    asm(x32k(x16("1:add (%[address]), %[reg]\n\t"
                 "add $8, %[address]\n\t")
             "jmp 1f\n\t"
             "nop\n\t"
             x16("1:add (%[address]), %[reg]\n\t"
                 "add $8, %[address]\n\t")
             "jmp 1f\n\t"
             "nop\n\t"
             x16("1:add (%[address]), %[reg]\n\t"
                 "add $8, %[address]\n\t")
             "test %[address], %[address]\n\t"
             "jnz 1f\n\t"
             "nop\n\t")
        "1:nop\n\t"
        :[address]"+r"(ptr), [reg]"+r"(accum)
        :: "cc");
  }
  result.metric = (double)LOOP128M*(16*2*3 + 7)/(now_nsec()-t);


  strcpy(result.metricname, "BIPS");
  strcpy(result.function, __FUNCTION__);
  return result;
}


int main() {
  struct Result r = bigtable_analog();
  printf("%s %s=%.4f\n", r.function, r.metricname, r.metric);
}
