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

#if defined(__x86_64__)

#ifdef __avx512__
#define DEPENDENT_VECTOR_INT_ADDS(x)         \
  asm(x("vpaddd %%zmm1, %%zmm2, %%zmm2\n\t") \
      :::"%zmm2");

#define DEPENDENT_VECTOR_INT_ADD_SETS(x)               \
  asm(x("vpaddd %%zmm0, %%zmm8, %%zmm8\n\t"            \
        "vpaddd %%zmm1, %%zmm9, %%zmm9\n\t"            \
        "vpaddd %%zmm2, %%zmm10, %%zmm10\n\t"          \
        "vpaddd %%zmm3, %%zmm11, %%zmm11\n\t"          \
        "vpaddd %%zmm4, %%zmm12, %%zmm12\n\t"          \
        "vpaddd %%zmm5, %%zmm13, %%zmm13\n\t"          \
        "vpaddd %%zmm6, %%zmm14, %%zmm14\n\t"          \
        "vpaddd %%zmm7, %%zmm15, %%zmm15\n\t")         \
        :::"%zmm8", "%zmm9", "%zmm10", "%zmm11", \
         "%zmm12", "%zmm13", "%zmm14", "%zmm15");
#else
#define DEPENDENT_VECTOR_INT_ADDS(x)         \
  asm(x("vpaddd %%ymm1, %%ymm2, %%ymm2\n\t") \
      :::"%ymm2");

#define DEPENDENT_VECTOR_INT_ADD_SETS(x)               \
  asm(x("vpaddd %%ymm0, %%ymm8, %%ymm8\n\t"            \
        "vpaddd %%ymm1, %%ymm9, %%ymm9\n\t"            \
        "vpaddd %%ymm2, %%ymm10, %%ymm10\n\t"          \
        "vpaddd %%ymm3, %%ymm11, %%ymm11\n\t"          \
        "vpaddd %%ymm4, %%ymm12, %%ymm12\n\t"          \
        "vpaddd %%ymm5, %%ymm13, %%ymm13\n\t"          \
        "vpaddd %%ymm6, %%ymm14, %%ymm14\n\t"          \
        "vpaddd %%ymm7, %%ymm15, %%ymm15\n\t")         \
        :::"%ymm8", "%ymm9", "%ymm10", "%ymm11", \
         "%ymm12", "%ymm13", "%ymm14", "%ymm15");
#endif

#elif defined(__ppc64__)

#define DEPENDENT_VECTOR_INT_ADDS(x) abort(); // PPC TBA
#define DEPENDENT_VECTOR_INT_ADD_SETS(x) abort(); // PPC TBA

#elif defined(__aarch64__)

// TODO(zhaop): SVE needs to be tested when we gain access to ARM
// machines that support SVE
#ifdef __sve__
#define DEPENDENT_VECTOR_INT_ADDS(x)   \
  asm(x("add z1.4D, z1.4D, z1.4D\n\t") \
      :::"z1");

#define DEPENDENT_VECTOR_INT_ADD_SETS(x) \
  asm(x("add z0.4D, z0.4D, z8.4D\n\t"    \
        "add z1.4D, z1.4D, z9.4D\n\t"    \
        "add z2.4D, z2.4D, z10.4D\n\t"   \
        "add z3.4D, z3.4D, z11.4D\n\t"   \
        "add z4.4D, z4.4D, z12.4D\n\t"   \
        "add z5.4D, z5.4D, z13.4D\n\t"   \
        "add z6.4D, z6.4D, z14.4D\n\t"   \
        "add z7.4D, z7.4D, z15.4D\n\t")  \
        :::"z0", "z1", "z2", "z3",       \
         "z4", "z5", "z6", "z7");
#else  // neon

#define DEPENDENT_VECTOR_INT_ADDS(x)   \
  asm(x("add v1.2D, v1.2D, v1.2D\n\t") \
      :::"v1");

#define DEPENDENT_VECTOR_INT_ADD_SETS(x) \
  asm(x("add v0.2D, v0.2D, v8.2D\n\t"    \
        "add v1.2D, v1.2D, v9.2D\n\t"    \
        "add v2.2D, v2.2D, v10.2D\n\t"   \
        "add v3.2D, v3.2D, v11.2D\n\t"   \
        "add v4.2D, v4.2D, v12.2D\n\t"   \
        "add v5.2D, v5.2D, v13.2D\n\t"   \
        "add v6.2D, v6.2D, v14.2D\n\t"   \
        "add v7.2D, v7.2D, v15.2D\n\t")  \
        :::"v0", "v1", "v2", "v3",       \
         "v4", "v5", "v6", "v7");
#endif  // neon or sve
#endif

#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_VECTOR_H_
