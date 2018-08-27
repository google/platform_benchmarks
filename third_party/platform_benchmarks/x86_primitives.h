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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_X86_PRIMITIVES_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_X86_PRIMITIVES_H_

#define REP_MOVS_LOOP(outer_loop_count,                                 \
                      copy_count,                                       \
                      src_pointer,                                      \
                      dst_pointer,                                      \
                      preamble,                                         \
                      asmbody,                                          \
                      postamble)                                        \
  asm(preamble                                                          \
      "add $0, %[loop_counter]\n\t"                                     \
      ".align 64\n\t"                                                   \
      "1: jz 1f\n\t"                                                    \
      "mov %[src], %%rsi\n\t"                                           \
      "mov %[dst], %%rdi\n\t"                                           \
      "mov %[count], %%rcx\n\t"                                         \
      asmbody                                                           \
      "dec %[loop_counter]\n\t"                                         \
      "jmp 1b\n\t"                                                      \
      "1: nop\n\t"                                                      \
      postamble                                                         \
      :[loop_counter]"+r"(outer_loop_count)                             \
      : [src]"r"(src_pointer), [dst]"r"(dst_pointer),                   \
        [count]"r"(copy_count)                                          \
      : "%rcx", "%rsi", "%rdi", "cc", "memory");


#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_X86_PRIMITIVES_H_
