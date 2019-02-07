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

#ifndef PLATFORMS_BENCHMARKS_ALU_H_
#define PLATFORMS_BENCHMARKS_ALU_H_

#include "third_party/platform_benchmarks/util.h"

#if defined(__x86_64__)

#define DEPENDENT_ADDS(x, r)   asm(x("add $15, %0\n\t") \
                                   :"+r"(r)             \
                                   ::"cc");
#define DEPENDENT_ADD_SETS(x, r, r0, r1, r2, r3, r4, r5, r6, r7) \
  asm(x("add %8, %0\n\t"        \
        "add %8, %1\n\t"        \
        "add %8, %2\n\t"        \
        "add %8, %3\n\t"        \
        "add %8, %4\n\t"        \
        "add %8, %5\n\t"        \
        "add %8, %6\n\t"        \
        "add %8, %7\n\t")       \
      :"+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3), \
       "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)       \
      : "r"(r)                  \
      : "cc");
#define DEPENDENT_SHL_REG(x, r1, r2) asm(x("shlq %1, %0\n\t") \
                                         :"+r"(r1)            \
                                         :"c"(r2)             \
                                         :"cc");
#define DEPENDENT_SHL_IMM(x, r, imm) asm(x("shlq $"#imm", %0\n\t")     \
                                         :"+r"(r)            \
                                         ::"cc");

#define DEPENDENT_CMOV_REG(x, r1, r2)           \
  asm("xor %0, %0\n\t"                          \
      x("cmovz %0, %1\n\tcmovz %1, %0\n\t")     \
      :"+r"(r1), "+r"(r2)                       \
      :: "cc");

#define MOV_ELIMINATION_SINGLE(a1, a2, count)           \
  asm(".align 64\n\t"                                   \
      "1:\n\t"                                          \
      "add %[r2], %[r1]\n\t"                            \
      "mov %[r1], %[r2]\n\t"                            \
      "dec %[count]\n\t"                                \
      "jnz 1b\n"                                        \
      :[count]"+r"(count), [r1]"+r"(a1), [r2]"+r"(a2)   \
      :: "cc");

#define MOV_ELIMINATION_1K(a1, a2, count) \
  asm(".align 64\n\t"                                   \
      "1:\n\t"                                          \
      x1k("add %[r2], %[r1]\n\tmov %[r1], %[r2]\n\t")   \
      "dec %[count]\n\t"                                \
      "jnz 1b\n"                                        \
      :[count]"+r"(count), [r1]"+r"(a1), [r2]"+r"(a2)   \
      :: "cc");                                         \

#elif defined(__ppc64__)

#define DEPENDENT_ADDS(x, r)   asm(x("addi %0, %0, 15\n\t") \
                                   :"+r"(r)             \
                                   ::"cc");

#define DEPENDENT_ADD_SETS(x, r, r0, r1, r2, r3, r4, r5, r6, r7) \
  asm(x("add %0, %0, %8\n\t"   \
        "add %1, %1, %8\n\t"   \
        "add %2, %2, %8\n\t"   \
        "add %3, %3, %8\n\t"   \
        "add %4, %4, %8\n\t"   \
        "add %5, %5, %8\n\t"   \
        "add %6, %6, %8\n\t"   \
        "add %7, %7, %8\n\t")  \
      :"+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3), \
       "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)  \
      : "r"(r)        \
      : "cc");

#define DEPENDENT_SHL_REG(x, r1, r2) abort();  // x86-only ppc
#define DEPENDENT_SHL_IMM(x, r, imm) abort();  // x86-only ppc
#define DEPENDENT_CMOV_REG(x, r1, r2) abort(); // x86-only ppc
#define MOV_ELIMINATION_SINGLE(a1, a2, count) abort();  // unimplemented ppc
#define MOV_ELIMINATION_1K(a1, a2, count) abort();  // unimplemented ppc

#elif defined(__aarch64__)

#define DEPENDENT_ADDS(x, r)   asm(x("add %0, %0, 15\n\t") \
                                   :"+r"(r)             \
                                   ::"cc");

#define DEPENDENT_ADD_SETS(x, r, r0, r1, r2, r3, r4, r5, r6, r7) \
  asm(x("add %0, %0, %8\n\t"                                     \
        "add %1, %1, %8\n\t"                                     \
        "add %2, %2, %8\n\t"                                     \
        "add %3, %3, %8\n\t"                                     \
        "add %4, %4, %8\n\t"                                     \
        "add %5, %5, %8\n\t"                                     \
        "add %6, %6, %8\n\t"                                     \
        "add %7, %7, %8\n\t")                                    \
      :"+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),                   \
       "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)                    \
      : "r"(r)                                                   \
      : "cc");

#define DEPENDENT_SHL_REG(x, r1, count) asm(x("lsl %0, %0, %1\n\t") \
                                         :"+r"(r1)                  \
                                         :"r"(count)                \
                                         :"cc");

#define DEPENDENT_SHL_IMM(x, r, imm) asm(x("lsl %0, %0, #"#imm"\n\t") \
                                         :"+r"(r)            	      \
                                         ::"cc");

#define DEPENDENT_CMOV_REG(x, r1, r2)                     \
  asm("eor %0, %0, %0\n\t"                                \
      x("csel %0, %0, %1, EQ\n\tcsel %1, %0, %1, EQ\n\t") \
      :"+r"(r1), "+r"(r2)                                 \
      :: "cc");

#define MOV_ELIMINATION_SINGLE(a1, a2, count)         \
  asm(".balign 64\n\t"                                \
      "1:\n\t"                                        \
      "add %[r2], %[r1], %[r2]\n\t"                   \
      "mov %[r1], %[r2]\n\t"                          \
      "subs %[count], %[count], #1\n\t"                \
      "bne 1b\n"                                      \
      :[count]"+r"(count), [r1]"+r"(a1), [r2]"+r"(a2) \
      :: "cc");

#define MOV_ELIMINATION_1K(a1, a2, count)                    \
  asm(".balign 64\n\t"                                       \
      "1:\n\t"                                               \
      x1k("add %[r2], %[r1], %[r2]\n\tmov %[r1], %[r2]\n\t") \
      "subs %[count], %[count], #1\n\t"                       \
      "bne 1b\n"                                             \
      :[count]"+r"(count), [r1]"+r"(a1), [r2]"+r"(a2)        \
      :: "cc");

#endif

#endif  // PLATFORMS_BENCHMARKS_ALU_H_
