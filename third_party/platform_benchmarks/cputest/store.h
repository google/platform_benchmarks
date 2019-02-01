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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_STORE_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_STORE_H_

#include "third_party/platform_benchmarks/util.h"

#define STORE 0

#ifdef __x86_64__
// Need to specify destination explicitly
// Otherwise assembler sets up a pointer chase
#define SCALAR_STORES(x, m)    asm(x("mov %%rdx, %0;")   \
                                   : "=m"(*m)             \
                                   :: );

#define VECTOR_STORES(x, m)    asm(x("vmovapd %%ymm0, %0;") \
                                   : "=m"(*m)                \
                                   :: );

#define VECTORSTOREx86(offset, index, ymmreg) \
  "vmovapd %%"#ymmreg", "#offset"(%["#index"])\n\t"

#define VMOVAPD_STORE(offset, index, ymmreg) \
  "vmovapd %%"#ymmreg", "#offset"(%["#index"])\n\t"

#define VMOVUPD_STORE(offset, index, ymmreg) \
  "vmovupd %%"#ymmreg", "#offset"(%["#index"])\n\t"

#define SCALAR_STORE_WIDTH 8
#define VECTOR_STORE_WIDTH 32

#elif defined(__ppc64__)

// Need to specify destination explicitly
// Otherwise assembler sets up a pointer chase
#define SCALAR_STORES(x, m)    asm(x("std %%r4, %0;")  \
                                   : "=m"(*m) \
                                   :: );

#define VECTOR_STORES(x, m)    asm("li %%r3, 0;\n\t"               \
                                  x("stvx %%v0, %0, %%r3;\n\t") \
                                  :: "r"(m) \
                                  : "%r3", "memory");

#define VECTORSTOREppc(r0, r1, v) "stvx %%"#v", %["#r0"], %["#r1"]\n\t"

#define SCALAR_STORE_WIDTH 8
#define VECTOR_STORE_WIDTH 16

#elif defined(__aarch64__)

#define SCALAR_STORES(x, m) \
  asm(x("str x4, %0\n\t") \
      : "=m"(*m)          \
      :: );

#define VECTOR_STORES(x, m)    asm(x("st1 {v0.16b}, %0\n\t") \
                                   :"=m"(*m)                 \
                                   ::);

#define VECTORSTOREarm(index, post_index, vectorreg)            \
  "st1 {"#vectorreg".16b}, [%["#index"]], %[post_index]\n\t"

#define SCALAR_STORE_WIDTH 8
#define VECTOR_STORE_WIDTH 16

#endif

#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_STORE_H_
