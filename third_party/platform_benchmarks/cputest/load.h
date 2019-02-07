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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_LOAD_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_LOAD_H_

#include "third_party/platform_benchmarks/util.h"

#define LOAD 1

#if defined(__x86_64__)

// Need to specify destination explicitly
// Otherwise assembler sets up a pointer chase
#define SCALAR_LOADS(x, m)    asm(x("mov %0, %%rdx;") \
                                  :: "m"(*m)          \
                                  : "%rdx");

#define VECTOR_LOADS(x, m)    asm(x("vmovapd %0, %%ymm0;") \
                                  :: "m"(*m)               \
                                  : "%ymm0");

#define VECTORLOADx86(offset, index, ymmreg) \
  "vmovapd "#offset"(%["#index"]), %%"#ymmreg"\n\t"

#define VMOVAPD_LOAD(offset, index, ymmreg) \
  "vmovapd "#offset"(%["#index"]), %%"#ymmreg"\n\t"

#define VMOVUPD_LOAD(offset, index, ymmreg) \
  "vmovupd "#offset"(%["#index"]), %%"#ymmreg"\n\t"


#define SCALAR_LOAD_WIDTH 8
#define VECTOR_LOAD_WIDTH 32

#elif defined(__ppc64__)

// Need to specify destination explicitly
// Otherwise assembler sets up a pointer chase
#define SCALAR_LOADS(x, m)    asm(x("ld %%r4, %0;") \
                                  :: "m"(*m)        \
                                  : "%r4");

#define VECTOR_LOADS(x, m)    asm("li %%r3, 0;\n\t"            \
                                  x("lvx %%v0, %0, %%r3;\n\t") \
                                  :: "r"(m) \
                                  : "%v0", "%r3");

#define VECTORLOADppc(r0, r1, v) "lvx %%"#v", %["#r0"], %["#r1"]\n\t"

#define SCALAR_LOAD_WIDTH 8
#define VECTOR_LOAD_WIDTH 16

#elif defined(__aarch64__)

#define SCALAR_LOADS(x, m) \
  asm(x("ldr x4, %0\n\t")  \
      :: "m"(*m)           \
      : "%x4");

#define VECTOR_LOADS(x, m)   asm(x("ld1 {v0.16b}, %0\n\t") \
                                 ::"m"(*m)                 \
                                 : "cc");

#define VECTORLOADarm(index, post_index, vectorreg) \
  "ld1 {"#vectorreg".16b}, [%["#index"]], %[post_index]\n\t"

#define SCALAR_LOAD_WIDTH 8
#define VECTOR_LOAD_WIDTH 16
#endif

#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_LOAD_H_
