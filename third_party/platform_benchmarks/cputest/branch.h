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
// We choose to use chained if statements instead of switch to avoid some
// assemblers complaining out-our-range conditional brarnches.
//
// Due to the limited conditional branch range in certain architectures,
// we control the loops containing large kernels duplication factors with
// unconditional branches. Other architectures take similar treatments for
// fair comparison.
#define BRANCH_SWEEP(BRANCHTYPE, logsize, lc, m)                     \
{                                                                    \
  assert(logsize <= 16);                                             \
  if(logsize == 0) {                                                 \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP0, lc, m, x1); }    \
  else if(logsize == 1) {                                            \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP1, lc, m, x2); }    \
  else if(logsize == 2) {                                            \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP2, lc, m, x4); }    \
  else if(logsize == 3) {                                            \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP3, lc, m, x8); }    \
  else if (logsize == 4) {                                           \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP4, lc, m, x16); }   \
  else if (logsize == 5) {                                           \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP5, lc, m, x32); }   \
  else if (logsize == 6) {                                           \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP6, lc, m, x64); }   \
  else if (logsize == 7) {                                           \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP7, lc, m, x128); }  \
  else if (logsize == 8) {                                           \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP8, lc, m, x256); }  \
  else if (logsize == 9) {                                           \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP9, lc, m, x512); }  \
  else if (logsize == 10) {                                          \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP10, lc, m, x1k); }  \
  else if (logsize == 11) {                                          \
    BRANCHTYPE##_SMALL_KERNEL(BRANCHTYPE ## SWEEP11, lc, m, x2k); }  \
  else if (logsize == 12) {                                          \
    BRANCHTYPE##_LARGE_KERNEL(BRANCHTYPE ## SWEEP12, lc, m, x4k); }  \
  else if (logsize == 13) {                                          \
    BRANCHTYPE##_LARGE_KERNEL(BRANCHTYPE ## SWEEP13, lc, m, x8k); }  \
  else if (logsize == 14) {                                          \
    BRANCHTYPE##_LARGE_KERNEL(BRANCHTYPE ## SWEEP14, lc, m, x16k); } \
  else if (logsize == 15) {                                          \
    BRANCHTYPE##_LARGE_KERNEL(BRANCHTYPE ## SWEEP15, lc, m, x32k); } \
  else if (logsize == 16) {                                          \
    BRANCHTYPE##_LARGE_KERNEL(BRANCHTYPE ## SWEEP16, lc, m, x64k);   \
  }                                                                  \
}

#if defined(__x86_64__)

#define TAKEN_BRANCH_LOOP(loop_count)     asm(".align 32\n"                 \
                                              "loop_start_takenbranch:\n\t" \
                                              "dec %0\n\t"                  \
                                              "jnz loop_start_takenbranch"  \
                                              : "+r"(loop_count)            \
                                              :: "cc");


#define REG_INDIRECT_BRANCH_LOOP(lc, tb, sr, mr, er)   \
{                                                      \
  asm(".align 32\n\r"                                  \
      "lea loop_start_indirectbranch, %[startreg]\n\t" \
      "lea loop_mid_indirectbranch, %[midreg]\n\t"     \
      "lea loop_exit_indirectbranch, %[exitreg]\n\t"   \
      "loop_start_indirectbranch:\n\t"                 \
      "nop\n\t"                                        \
      "loop_mid_indirectbranch:\n\t"                   \
      "movq %[startreg], %[target_branch]\n\t"         \
      "test $1, %[loop_count]\n\t"                     \
      "cmovzq %[midreg], %[target_branch]\n\t"         \
      "dec %[loop_count]\n\t"                          \
      "cmovzq %[exitreg], %[target_branch]\n\t"        \
      "jmpq *%[target_branch]\n\t"                     \
      "loop_exit_indirectbranch:\n\t"                  \
      "nop"                                            \
      : [loop_count]"+r"(lc),                          \
        [target_branch]"+r"(tb),                       \
        [startreg]"+r"(sr),                            \
        [midreg]"+r"(mr),                              \
        [exitreg]"+r"(er)                              \
      :: "cc");                                        \
}


#define BRANCH_HISTORY(lc, idx, dm, m, result)                 \
  asm("loop_start_branch_history:\n\t"                         \
      "mov %[loop_count], %[index]\n\t"                        \
      "and %[depth_mask], %[index]\n\t"                        \
      "cmpq $0, (%[mem],%[index],8)\n\t"                       \
      "jnz branch_history_there\n\t"                           \
      "add %[loop_count], %[x]\n\t"                            \
      "branch_history_there:\n\t"                              \
      "dec %[loop_count]\n\t"                                  \
      "jnz loop_start_branch_history\n"                        \
      :[loop_count]"+r"(lc), [index]"+r"(idx), [x]"+r"(result) \
      :[mem]"r"(m), [depth_mask]"r"(dm)                        \
      : "cc");

#define UNCOND_DIRECT_BRANCHES_SMALL_KERNEL(callsite, lc, m, x) \
{                                                               \
  asm(".align 4096\n\t"                                         \
      "start_loop_"#callsite":\n\t"                             \
      x("1: jmp 1f\n\tnop\n\t")                                 \
      "1: dec %[loop_count]\n\t"                                \
      "jnz start_loop_"#callsite                                \
      :[loop_count]"+r"(lc)                                     \
      :: "cc");                                                 \
}

#define UNCOND_DIRECT_BRANCHES_LARGE_KERNEL(callsite, lc, m, x) \
{                                                               \
  asm(".align 4096\n\t"                                         \
      "start_loop_"#callsite":\n\t"                             \
      x("1: jmp 1f\n\tnop\n\t")                                 \
      "1: dec %[loop_count]\n\t"                                \
      "jz exit_loop_"#callsite"\n\t"                            \
      "jmp start_loop_"#callsite"\n\t"                          \
      "exit_loop_"#callsite":\n\t"                              \
      "nop"                                                     \
      :[loop_count]"+r"(lc)                                     \
      :: "cc");                                                 \
}

#define COND_BRANCHES_SMALL_KERNEL(callsite, lc, m, x) \
{                                                      \
  asm(".align 4096\n\t"                                \
      "start_loop_"#callsite":\n\t"                    \
      "test %[mask], %[loop_count]\n\t"                \
      x("1: jz 1f\n\tnop\n\t")                         \
      "1: dec %[loop_count]\n\t"                       \
      "jnz start_loop_"#callsite                       \
      :[loop_count]"+r"(lc)                            \
      :[mask]"r"(m)                                    \
      : "cc");                                         \
}

#define COND_BRANCHES_LARGE_KERNEL(callsite, lc, m, x) \
{                                                      \
  asm(".align 4096\n\t"                                \
      "start_loop_"#callsite":\n\t"                    \
      "test %[mask], %[loop_count]\n\t"                \
      x("1: jz 1f\n\tnop\n\t")                         \
      "1: dec %[loop_count]\n\t"                       \
      "jz exit_loop_"#callsite"\n\t"                   \
      "jmp start_loop_"#callsite"\n\t"                 \
      "exit_loop_"#callsite":\n\t"                     \
      "nop"                                            \
      :[loop_count]"+r"(lc)                            \
      :[mask]"r"(m)                                    \
      : "cc");                                         \
}

#elif defined(__ppc64__)

#define TAKEN_BRANCH_LOOP(loop_count)                              \
  asm("mtctr %[loop_count]\n\t"                                    \
      "loop_start_takenbranch:\n\t"                                \
      "bdnz loop_start_takenbranch" ::[loop_count] "r"(loop_count) \
      : "cc", "ctr");

#define REG_INDIRECT_BRANCH_LOOP(lc, tb, sr, mr, er)                                           \
{ register uint64_t tmp = 0;                                                                   \
  asm(".balign 32\n\t"                                                                         \
      "lis %[startreg], loop_start_indirectbranch@highest\n\t"                                 \
      "ori %[startreg], %[startreg], loop_start_indirectbranch@higher\n\t"                     \
      "sldi %[startreg], %[startreg], 32\n\t"                                                  \
      "oris %[startreg], %[startreg], loop_start_indirectbranch@h\n\t"                         \
      "ori %[startreg], %[startreg], loop_start_indirectbranch@l\n\t"                          \
      "addi %[midreg], %[startreg], loop_mid_indirectbranch - loop_start_indirectbranch\n\t"   \
      "addi %[exitreg], %[startreg], loop_exit_indirectbranch - loop_start_indirectbranch\n\t" \
      "loop_start_indirectbranch:\n\t"                                                         \
      "nop\n\t"                                                                                \
      "loop_mid_indirectbranch:\n\t"                                                           \
      "mr %[target_branch], %[startreg]\n\t"                                                   \
      "andi. %[tmpr], %[loop_count], 1\n\t"                                                    \
      "iseleq %[target_branch], %[midreg], %[target_branch]\n\t"                               \
      "subic. %[loop_count], %[loop_count], 1\n\t"                                             \
      "iseleq %[target_branch], %[exitreg], %[target_branch]\n\t"                              \
      "mtctr %[target_branch]\n\t"                                                             \
      "bctrl\n\t"                                                                              \
      "loop_exit_indirectbranch:\n\t"                                                          \
      "nop"                                                                                    \
      : [loop_count]"+r"(lc),                                                                  \
        [target_branch]"+r"(tb),                                                               \
        [startreg]"+r"(sr),                                                                    \
        [midreg]"+r"(mr),                                                                      \
        [exitreg]"+r"(er),                                                                     \
        [tmpr]"+r"(tmp)                                                                        \
      :: "cc");                                                                                \
}

#define BRANCH_HISTORY(lc, idx, dm, m, result)                     \
{                                                                  \
  asm("mtctr %[loop_count]\n\t"                                    \
      "loop_start_branch_history:\n\t"                             \
      "and %[index], %[depth_mask], %[loop_count]\n\t"             \
      "sldi %%r15, %[index], 3\n\t"                                \
      "ldx %%r15, %%r15, %[mem]\n\t"                               \
      "cmpdi %%cr7, %%r15, 0\n\t"                                  \
      "beq %%cr7, branch_history_there\n\t"                        \
      "add %[x], %[x], %[loop_count]\n\t"                          \
      "nop\n\t"                                                    \
      "branch_history_there:\n\t"                                  \
      "subi %[loop_count], %[loop_count], 1\n\t"                   \
      "bdnz loop_start_branch_history\n\t"                         \
      : [loop_count] "+r"(lc), [index] "+r"(idx), [x] "+r"(result) \
      : [mem] "r"(m), [depth_mask] "r"(dm)                         \
      : "%r15", "cc", "ctr");                                      \
}

#define UNCOND_DIRECT_BRANCHES_SMALL_KERNEL(callsite, lc, m, x) \
{                                                               \
  asm(".balign 4096\n\t"                                        \
      "start_loop_"#callsite":\n\t"                             \
      x("1: b 1f\n\tnop\n\t")                                   \
      "1: subic. %[loop_count], %[loop_count], 1\n\t"           \
      "bne start_loop_"#callsite                                \
      :[loop_count]"+r"(lc)                                     \
      :: "cc");                                                 \
}

#define UNCOND_DIRECT_BRANCHES_LARGE_KERNEL(callsite, lc, m, x)                     \
{ register uint64_t tmp = 0;                                                        \
  asm(".balign 4096\n\t"                                                            \
      "lis %[starting_label], start_loop_"#callsite"@highest\n\t"                   \
      "ori %[starting_label], %[starting_label], start_loop_"#callsite"@higher\n\t" \
      "sldi %[starting_label], %[starting_label], 32\n\t"                           \
      "oris %[starting_label], %[starting_label], start_loop_"#callsite"@h\n\t"     \
      "ori %[starting_label], %[starting_label], start_loop_"#callsite"@l\n\t"      \
      "mtctr %[starting_label]\n\t"                                                 \
      "start_loop_"#callsite":\n\t"                                                 \
      x("1: b 1f\n\tnop\n\t")                                                       \
      "1: subic. %[loop_count], %[loop_count], 1\n\t"                               \
      "beq exit_loop_"#callsite"\n\t"                                               \
      "bctr\n\t"                                                                    \
      "exit_loop_"#callsite":\n\t"                                                  \
      "nop\n\t"                                                                     \
      :[loop_count]"+r"(lc), [starting_label]"+r"(tmp)                              \
      :: "cc");                                                                     \
}

#define COND_BRANCHES_SMALL_KERNEL(callsite, lc, m, repetition) \
{                                                               \
  asm(".balign 4096\n\t"                                        \
      "start_loop_"#callsite":\n\t"                             \
      "cmpld %[mask], %[loop_count]\n\t"                        \
      repetition("1: beq 1f\n\tnop\n\t")                        \
      "1: subic. %[loop_count], %[loop_count], 1\n\t"           \
      "bne start_loop_"#callsite                                 \
      :[loop_count]"+r"(lc)                                     \
      :[mask]"r"(m)                                             \
      : "cc");                                                  \
}

#define COND_BRANCHES_LARGE_KERNEL(callsite, lc, m, repetition)                     \
{ register uint64_t tmp = 0;                                                        \
  asm(".balign 4096\n\t"                                                            \
      "lis %[starting_label], start_loop_"#callsite"@highest\n\t"                   \
      "ori %[starting_label], %[starting_label], start_loop_"#callsite"@higher\n\t" \
      "sldi %[starting_label], %[starting_label], 32\n\t"                           \
      "oris %[starting_label], %[starting_label], start_loop_"#callsite"@h\n\t"     \
      "ori %[starting_label], %[starting_label], start_loop_"#callsite"@l\n\t"      \
      "mtctr %[starting_label]\n\t"                                                 \
      "start_loop_"#callsite":\n\t"                                                 \
      "cmpld %[mask], %[loop_count]\n\t"                                            \
      repetition("1: beq 1f\n\tnop\n\t")                                            \
      "1: subic. %[loop_count], %[loop_count], 1\n\t"                               \
      "beq exit_loop_"#callsite"\n\t"                                               \
      "bctr\n\t"                                                                    \
      "exit_loop_"#callsite":\n\t"                                                  \
      "nop\n\t"                                                                     \
      :[loop_count]"+r"(lc), [starting_label]"+r"(tmp)                              \
      :[mask]"r"(m)                                                                 \
      : "cc");                                                                      \
}

#elif defined(__aarch64__)

#define TAKEN_BRANCH_LOOP(loop_count)       \
  asm("loop_start_takenbranch:\n\t"         \
      "sub %0, %0, 1\n\t"                   \
      "cbnz %0, loop_start_takenbranch\n\t" \
      : "+r"(loop_count)                    \
      :: "cc");

#define BRANCH_HISTORY(lc, idx, dm, m, result)                 \
  asm(".balign 4096\n\t"                                       \
      "loop_start_branch_history:\n\t"                         \
      "and %[index], %[depth_mask], %[loop_count]\n\t"         \
      "lsl x15, %[index], #3\n\t"                              \
      "add x15, x15, %[mem]\n\t"                               \
      "ldr x15, [x15, #0]\n\t"                                 \
      "cmp x15, #0\n\t"                                        \
      "beq branch_history_there\n\t"                           \
      "add %[x], %[x], %[loop_count]\n\t"                      \
      "branch_history_there:\n\t"                              \
      "subs %[loop_count], %[loop_count], #1\n\t"              \
      "bne loop_start_branch_history\n"                        \
      :[loop_count]"+r"(lc), [index]"+r"(idx), [x]"+r"(result) \
      :[mem]"r"(m), [depth_mask]"r"(dm)                        \
      : "%x15", "cc");

#define UNCOND_DIRECT_BRANCHES_SMALL_KERNEL(callsite, lc, m, x) \
  asm(".balign 4096\n\t"                                        \
      "start_loop_"#callsite":\n\t"                             \
      x("1: b 1f\n\tnop\n\t")                                   \
      "1: subs %[loop_count], %[loop_count], #1\n\t"            \
      "bne start_loop_"#callsite                                \
      :[loop_count]"+r"(lc)                                     \
      :: "cc");

#define UNCOND_DIRECT_BRANCHES_LARGE_KERNEL(callsite, lc, m, x) \
  asm(".balign 4096\n\t"                                        \
      "start_loop_"#callsite":\n\t"                             \
      x("1: b 1f\n\tnop\n\t")                                   \
      "1: subs %[loop_count], %[loop_count], #1\n\t"            \
      "beq exit_loop_"#callsite"\n\t"                           \
      "b start_loop_"#callsite"\n\t"                            \
      "exit_loop_"#callsite":\n\t"                              \
      "nop"                                                     \
      :[loop_count]"+r"(lc)                                     \
      :: "cc");

#define COND_BRANCHES_SMALL_KERNEL(callsite, lc, m, x) \
  asm(".balign 4096\n\t"                               \
      "start_loop_"#callsite":\n\t"                    \
      "tst %[mask], %[loop_count]\n\t"                 \
      x("1: beq 1f\n\tnop\n\t")                        \
      "1: subs %[loop_count], %[loop_count], #1\n\t"   \
      "bne start_loop_"#callsite                       \
      :[loop_count]"+r"(lc)                            \
      :[mask]"r"(m)                                    \
      : "cc");

#define COND_BRANCHES_LARGE_KERNEL(callsite, lc, m, x) \
  asm(".balign 4096\n\t"                               \
      "start_loop_"#callsite":\n\t"                    \
      "tst %[mask], %[loop_count]\n\t"                 \
      x("1: beq 1f\n\tnop\n\t")                        \
      "1: subs %[loop_count], %[loop_count], #1\n\t"   \
      "beq exit_loop_"#callsite"\n\t"                  \
      "b start_loop_"#callsite"\n\t"                   \
      "exit_loop_"#callsite":\n\t"                     \
      "nop"                                            \
      :[loop_count]"+r"(lc)                            \
      :[mask]"r"(m)                                    \
      : "cc");

#define REG_INDIRECT_BRANCH_LOOP(lc, tb, sr, mr, er)                \
  asm(".balign 32\n\t"                                              \
      "adr %[startreg], loop_start_indirectbranch\n\t"              \
      "adr %[midreg], loop_mid_indirectbranch\n\t"                  \
      "adr %[exitreg], loop_exit_indirectbranch\n\t"                \
      "loop_start_indirectbranch:\n\t"                              \
      "nop\n\t"                                                     \
      "loop_mid_indirectbranch:\n\t"                                \
      "mov %[target_branch], %[startreg]\n\t"                       \
      "tst %[loop_count], #1\n\t"                                   \
      "csel %[target_branch], %[midreg], %[target_branch], EQ\n\t"  \
      "subs %[loop_count], %[loop_count], #1\n\t"                   \
      "csel %[target_branch], %[exitreg], %[target_branch], EQ\n\t" \
      "br %[target_branch]\n\t"                                     \
      "loop_exit_indirectbranch:\n\t"                               \
      "nop"                                                         \
      : [loop_count]"+r"(lc),                                       \
        [target_branch]"+r"(tb),                                    \
        [startreg]"+r"(sr),                                         \
        [midreg]"+r"(mr),                                           \
        [exitreg]"+r"(er)                                           \
      :: "cc");

#endif

#endif  // PLATFORMS_BENCHMARKS_BRANCH_H_
