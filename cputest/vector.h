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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_VECTOR_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_VECTOR_H_

#include <stdlib.h>
#include "third_party/platform_benchmarks/util.h"

#ifdef __x86_64__
#define DEPENDENT_VECTOR256_INT_ADDS(x)                                 \
  asm(x("vpaddd %%ymm1, %%ymm2, %%ymm2\n\t")                            \
      :::"cc", "%ymm2");

#define DEPENDENT_VECTOR256_INT_ADD_SETS(x)            \
  asm(x("vpaddd %%ymm0, %%ymm8, %%ymm8\n\t"            \
        "vpaddd %%ymm1, %%ymm9, %%ymm9\n\t"            \
        "vpaddd %%ymm2, %%ymm10, %%ymm10\n\t"          \
        "vpaddd %%ymm3, %%ymm11, %%ymm11\n\t"          \
        "vpaddd %%ymm4, %%ymm12, %%ymm12\n\t"          \
        "vpaddd %%ymm5, %%ymm13, %%ymm13\n\t"          \
        "vpaddd %%ymm6, %%ymm14, %%ymm14\n\t"          \
        "vpaddd %%ymm7, %%ymm15, %%ymm15\n\t")         \
        :::"cc", "%ymm8", "%ymm9", "%ymm10", "%ymm11", \
         "%ymm12", "%ymm13", "%ymm14", "%ymm15");
#endif

#ifdef __PPC64__
#define DEPENDENT_VECTOR256_INT_ADDS(x) abort();
#define DEPENDENT_VECTOR256_INT_ADD_SETS(x) abort();
#endif

#ifdef __aarch64__
#define DEPENDENT_VECTOR256_INT_ADDS(x) abort();
#define DEPENDENT_VECTOR256_INT_ADD_SETS(x) abort();
#endif

#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_VECTOR_H_
