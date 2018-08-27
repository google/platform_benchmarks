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

#ifndef PLATFORMS_BENCHMARKS_BRANCH_H_
#define PLATFORMS_BENCHMARKS_BRANCH_H_

#include "third_party/platform_benchmarks/util.h"

#define RAND_SEED 1220

// Macros work exactly for a single instantiation because of global labels

#define BRANCH_SWEEP(BRANCHTYPE, logs, lc, m)     \
  switch (logs) {    \
    case 0:          \
      BRANCHTYPE(BRANCHTYPE ## SWEEP0, lc, m, x1) \
      break;                                      \
    case 1: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP1, lc, m, x2) \
      break; \
    case 2: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP2, lc, m, x4) \
      break; \
    case 3: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP3, lc, m, x8) \
      break; \
    case 4: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP4, lc, m, x16) \
      break; \
    case 5: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP5, lc, m, x32) \
      break; \
    case 6: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP6, lc, m, x64) \
      break; \
    case 7: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP7, lc, m, x128) \
      break; \
    case 8: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP8, lc, m, x256) \
      break; \
    case 9: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP9, lc, m, x512) \
      break; \
    case 10: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP10, lc, m, x1k) \
      break; \
    case 11: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP11, lc, m, x2k) \
      break; \
    case 12: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP12, lc, m, x4k) \
      break; \
    case 13: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP13, lc, m, x8k) \
      break; \
    case 14: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP14, lc, m, x16k) \
      break; \
    case 15: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP15, lc, m, x32k) \
      break; \
    case 16: \
      BRANCHTYPE(BRANCHTYPE ## SWEEP16, lc, m, x64k) \
      break; \
    default: \
      assert(logs <= 16); \
  }

#ifdef __x86_64__
#define TAKEN_BRANCH_LOOP(loop_count)     asm(".align 32\n" \
                                              "loop_start_takenbranch:\n\t" \
                                              "dec %0\n\t" \
                                              "jnz loop_start_takenbranch" \
                                              : "+r"(loop_count) \
                                              :: "cc");


#define REG_INDIRECT_BRANCH_LOOP(lc, tb, sr, mr, er)                    \
  asm(".align 32\n\r"                                                   \
      "lea loop_start_indirectbranch, %[startreg]\n\t"                  \
      "lea loop_mid_indirectbranch, %[midreg]\n\t"                      \
      "lea loop_exit_indirectbranch, %[exitreg]\n\t"                    \
      "loop_start_indirectbranch:\n\t"                                  \
      "nop\n\t"                                                         \
      "loop_mid_indirectbranch:\n\t"                                    \
      "movq %[startreg], %[target_branch]\n\t"                          \
      "test $1, %[loop_count]\n\t"                                      \
      "cmovzq %[midreg], %[target_branch]\n\t"                          \
      "dec %[loop_count]\n\t"                                           \
      "cmovzq %[exitreg], %[target_branch]\n\t"                         \
      "jmpq *%[target_branch]\n\t"                                      \
      "loop_exit_indirectbranch:\n\t"                                   \
      "nop"                                                             \
      : [loop_count]"+r"(lc),                                           \
        [target_branch]"+r"(tb),                                        \
        [startreg]"+r"(sr),                                             \
        [midreg]"+r"(mr),                                               \
        [exitreg]"+r"(er)                                               \
      :: "cc");


#define BRANCH_HISTORY(lc, idx, dm, m, result)         \
  asm("loop_start_branch_history:\n\t" \
      "mov %[loop_count], %[index]\n\t"  \
      "and %[depth_mask], %[index]\n\t"  \
      "cmpq $0, (%[mem],%[index],8)\n\t" \
      "jnz branch_history_there\n\t"     \
      "add %[loop_count], %[x]\n\t"     \
      "branch_history_there:\n\t"        \
      "dec %[loop_count]\n\t"            \
      "jnz loop_start_branch_history\n" \
      :[loop_count]"+r"(lc), [index]"+r"(idx), [x]"+r"(result)  \
      :[mem]"r"(m), [depth_mask]"r"(dm)  \
      : "cc");

#define UNCOND_DIRECT_BRANCHES(callsite, lc, m, x)                   \
  asm(".align 4096\n\t"                                              \
      "start_loop_"#callsite":\n\t"                                  \
      x("1: jmp 1f\n\tnop\n\t")                                      \
      "1: dec %[loop_count]\n\t"                                     \
      "jnz start_loop_"#callsite                                     \
      :[loop_count]"+r"(lc)                                          \
      :: "cc");

#define COND_BRANCHES(callsite, lc, m, x)                            \
  asm(".align 4096\n\t"                                              \
      "start_loop_"#callsite":\n\t"                                  \
      "test %[mask], %[loop_count]\n\t"                              \
      x("1: jz 1f\n\tnop\n\t")                                       \
      "1: dec %[loop_count]\n\t"                                     \
      "jnz start_loop_"#callsite                                     \
      :[loop_count]"+r"(lc)                                          \
      :[mask]"r"(m)                                                  \
      : "cc");


#endif

#ifdef __PPC64__
#define TAKEN_BRANCH_LOOP(loop_count)                              \
  asm("mtctr %[loop_count]\n\t"                                    \
      "loop_start_takenbranch:\n\t"                                \
      "bdnz loop_start_takenbranch" ::[loop_count] "r"(loop_count) \
      : "cc", "ctr");

#define REG_INDIRECT_BRANCH_LOOP(lc, tb, sr, mr, er) assert(0);  // !implemented

#define BRANCH_HISTORY(lc, idx, dm, m, result)                     \
  asm("mtctr %[loop_count]\n\t"                                    \
      "loop_start_branch_history:\n\t"                              \
      "and %[index], %[depth_mask], %[loop_count]\n\t"             \
      "sldi %%r15, %[index], 3\n\t"                                \
      "ldx %%r15, %%r15, %[mem]\n\t"                               \
      "cmpdi %%cr7, %%r15, 0\n\t"                                  \
      "beq %%cr7, branch_history_there\n\t"                        \
      "add %[x], %[x], %[loop_count]\n\t"                          \
      "nop\n\t"                                                    \
      "branch_history_there:\n\t"                                  \
      "subi %[loop_count], %[loop_count], 1\n\t"                   \
      "bdnz loop_start_branch_history\n\t"                          \
      : [loop_count] "+r"(lc), [index] "+r"(idx), [x] "+r"(result) \
      : [mem] "r"(m), [depth_mask] "r"(dm)                         \
      : "%r15", "cc", "ctr");

#define UNCOND_DIRECT_BRANCHES(callsite, lc, m, x) abort();  // !implemented
#define COND_BRANCHES(callsite, lc, m, x) abort();  // !implemented

#endif

#ifdef __aarch64__
#define TAKEN_BRANCH_LOOP(loop_count) \
  asm("loop_start_takenbranch:\n\t"                                     \
      "sub %0, %0, 1\n\t"                                               \
      "cbnz %0, loop_start_takenbranch\n\t"                             \
      : "+r"(loop_count)                                                \
      :: "cc");

#define REG_INDIRECT_BRANCH_LOOP(lc, tb, sr, mr, er) assert(0);  // !implemented

#define BRANCH_HISTORY(lc, idx, dm, m, result)  \
  asm("loop_start_branch_history:\n\t" \
      "and %[index], %[depth_mask], %[loop_count]\n\t" \
      "lsl x15, %[index], #3\n\t" \
      "add x15, x15, %[mem]\n\t" \
      "ldr x15, [x15, #0]\n\t" \
      "cmp x15, #0\n\t" \
      "b.eq branch_history_there\n\t" \
      "add %[x], %[x], %[loop_count]\n\t" \
      "branch_history_there:\n\t" \
      "sub %[loop_count], %[loop_count], #1\n\t" \
      "cmp %[loop_count], #0\n\t" \
      "b.ne loop_start_branch_history\n" \
      :[loop_count]"+r"(lc), [index]"+r"(idx), [x]"+r"(result)  \
      :[mem]"r"(m), [depth_mask]"r"(dm)  \
      : "%x15", "cc");

#define UNCOND_DIRECT_BRANCHES(callsite, lc, m, x) abort();  // !implemented
#define COND_BRANCHES(callsite, lc, m, x) abort();  // !implemented

#endif

#endif  // PLATFORMS_BENCHMARKS_BRANCH_H_
