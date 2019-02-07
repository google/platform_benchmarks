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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_LOADSTORE_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_LOADSTORE_H_

#include "third_party/platform_benchmarks/x86_primitives.h"

#define RAND_SEED 1220
#define MAX_LOG_COPYSIZE 31

#if defined(__x86_64__)

#define STLF_LOOP_CHASE(loopcnt, depth, readloc, startptr, ptr, loadreg) \
  asm("1:\n\t"                                                           \
      "mov %[startpointer], %[pointer]\n\t"                              \
      x##depth("mov (%[pointer]), %[pointer]\n\t")                       \
      "mov %[loopcount], (%[pointer])\n\t"                               \
      "add (%[readlocation]), %[loadregister]\n\t"                       \
      "dec %[loopcount]\n\t"                                             \
      "jnz 1b\n"                                                         \
      :[loopcount]"+r"(loopcnt), [loadregister]"+r"(loadreg),            \
       [pointer]"+r"(ptr), [startpointer]"+r"(startptr)                  \
      :[readlocation]"r"(readloc)                                        \
      :"cc");

#elif defined(__ppc64__)

#define STLF_LOOP_CHASE(loopcnt, depth, readloc, startptr, ptr, loadreg) \
  for (; loopcnt > 0; loopcnt--) {                                       \
    ptr = startptr;                                                      \
    x##depth(ptr = *((uint64_t *)ptr);)                                  \
    *((uint64_t *)ptr) = loopcnt;                                        \
    loadreg += *((uint64_t *)readloc);                                   \
  }

#elif defined(__aarch64__)

#define STLF_LOOP_CHASE(loopcnt, depth, readloc, startptr, ptr, loadreg) \
  for (; loopcnt > 0; loopcnt--) {                                       \
    ptr = startptr;                                                      \
    x##depth(ptr = *((uint64_t *)ptr);)                                  \
    *((uint64_t *)ptr) = loopcnt;                                        \
    loadreg += *((uint64_t *)readloc);                                   \
  }
#endif


#define AVX_32BYTE_COPY_LOOP(loop_counter,                              \
                             LD_OP,                                     \
                             ST_OP,                                     \
                             src_pointer,                               \
                             dst_pointer)                               \
    asm(".align 64\n\t"                                                 \
        "1:\n\t"                                                        \
        LD_OP(0x0, src, ymm0)                                           \
        ST_OP(0x0, dst, ymm0)                                           \
        "dec %[counter]\n\t"                                            \
        "jnz 1b\n"                                                      \
        :[counter]"+r"(loop_counter)                                    \
        :[src]"r"(src_pointer), [dst]"r"(dst_pointer)                   \
        :"%ymm0", "memory");

#define AVX_64BYTE_COPY_LOOP(loop_counter,                              \
                             LD_OP,                                     \
                             ST_OP,                                     \
                             src_pointer,                               \
                             dst_pointer)                               \
    asm(".align 64\n\t"                                                 \
        "1:\n\t"                                                        \
        LD_OP(0x0, src, ymm0)                                           \
        ST_OP(0x0, dst, ymm0)                                           \
        LD_OP(0x20, src, ymm0)                                          \
        ST_OP(0x20, dst, ymm0)                                          \
        "dec %[counter]\n\t"                                            \
        "jnz 1b\n"                                                      \
        :[counter]"+r"(loop_counter)                                    \
        :[src]"r"(src_pointer), [dst]"r"(dst_pointer)                   \
        :"%ymm0", "memory");

#define AVX_128BYTE_COPY_LOOP(loop_counter,                             \
                              LD_OP,                                    \
                              ST_OP,                                    \
                              src_pointer,                              \
                              dst_pointer)                              \
    asm(".align 64\n\t"                                                 \
        "1:\n\t"                                                        \
        LD_OP(0x0, src, ymm0)                                           \
        ST_OP(0x0, dst, ymm0)                                           \
        LD_OP(0x20, src, ymm0)                                          \
        ST_OP(0x20, dst, ymm0)                                          \
        LD_OP(0x40, src, ymm0)                                          \
        ST_OP(0x40, dst, ymm0)                                          \
        LD_OP(0x60, src, ymm0)                                          \
        ST_OP(0x60, dst, ymm0)                                          \
        "dec %[counter]\n\t"                                            \
        "jnz 1b\n"                                                      \
        :[counter]"+r"(loop_counter)                                    \
        :[src]"r"(src_pointer), [dst]"r"(dst_pointer)                   \
        :"%ymm0", "memory");

#define AVX_256BYTE_COPY_LOOP(loop_counter,                             \
                              LD_OP,                                    \
                              ST_OP,                                    \
                              src_pointer,                              \
                              dst_pointer)                              \
    asm(".align 64\n\t"                                                 \
        "1:\n\t"                                                        \
        LD_OP(0x0, src, ymm0)                                           \
        ST_OP(0x0, dst, ymm0)                                           \
        LD_OP(0x20, src, ymm0)                                          \
        ST_OP(0x20, dst, ymm0)                                          \
        LD_OP(0x40, src, ymm0)                                          \
        ST_OP(0x40, dst, ymm0)                                          \
        LD_OP(0x60, src, ymm0)                                          \
        ST_OP(0x60, dst, ymm0)                                          \
        LD_OP(0x80, src, ymm0)                                          \
        ST_OP(0x80, dst, ymm0)                                          \
        LD_OP(0xa0, src, ymm0)                                          \
        ST_OP(0xa0, dst, ymm0)                                          \
        LD_OP(0xc0, src, ymm0)                                          \
        ST_OP(0xc0, dst, ymm0)                                          \
        LD_OP(0xe0, src, ymm0)                                          \
        ST_OP(0xe0, dst, ymm0)                                          \
        "dec %[counter]\n\t"                                            \
        "jnz 1b\n"                                                      \
        :[counter]"+r"(loop_counter)                                    \
        :[src]"r"(src_pointer), [dst]"r"(dst_pointer)                   \
        :"%ymm0", "memory");


#define AVX_COPY_LOOP(outer_loop_idx_reg,                               \
                      outer_loop_cnt,                                   \
                      inner_loop_idx_reg,                               \
                      inner_loop_cnt,                                   \
                      src_ptr_reg,                                      \
                      src_ptr,                                          \
                      dst_ptr_reg,                                      \
                      dst_ptr,                                          \
                      LD_OP,                                            \
                      ST_OP)                                            \
  asm("mov %[outer_loop_count], %[outer_loop_index_reg]\n\t"            \
      "start_avx_copy_outer_loop_"#LD_OP"_"#ST_OP":\n\t"                \
      "mov %[inner_loop_count], %[inner_loop_index_reg]\n\t"            \
      "mov %[src_pointer], %[src_pointer_reg]\n\t"                      \
      "mov %[dst_pointer], %[dst_pointer_reg]\n\t"                      \
      "start_avx_copy_inner_loop_"#LD_OP"_"#ST_OP":\n\t"                \
      LD_OP(0x0, src_pointer_reg, ymm0)                                 \
      ST_OP(0x0, dst_pointer_reg, ymm0)                                 \
      LD_OP(0x20, src_pointer_reg, ymm0)                                \
      ST_OP(0x20, dst_pointer_reg, ymm0)                                \
      LD_OP(0x40, src_pointer_reg, ymm0)                                \
      ST_OP(0x40, dst_pointer_reg, ymm0)                                \
      LD_OP(0x60, src_pointer_reg, ymm0)                                \
      ST_OP(0x60, dst_pointer_reg, ymm0)                                \
      LD_OP(0x80, src_pointer_reg, ymm0)                                \
      ST_OP(0x80, dst_pointer_reg, ymm0)                                \
      LD_OP(0xa0, src_pointer_reg, ymm0)                                \
      ST_OP(0xa0, dst_pointer_reg, ymm0)                                \
      LD_OP(0xc0, src_pointer_reg, ymm0)                                \
      ST_OP(0xc0, dst_pointer_reg, ymm0)                                \
      LD_OP(0xe0, src_pointer_reg, ymm0)                                \
      ST_OP(0xe0, dst_pointer_reg, ymm0)                                \
      "add $0x100, %[src_pointer_reg]\n\t"                              \
      "add $0x100, %[dst_pointer_reg]\n\t"                              \
      "dec %[inner_loop_index_reg]\n\t"                                 \
      "jnz start_avx_copy_inner_loop_"#LD_OP"_"#ST_OP"\n\t"             \
      "dec %[outer_loop_index_reg]\n\t"                                 \
      "jnz start_avx_copy_outer_loop_"#LD_OP"_"#ST_OP"\n"               \
      :[outer_loop_index_reg]"+r"(outer_loop_idx_reg),                  \
       [inner_loop_index_reg]"+r"(inner_loop_idx_reg),                  \
       [src_pointer_reg]"+r"(src_ptr_reg),                              \
       [dst_pointer_reg]"+r"(dst_ptr_reg)                               \
      :[outer_loop_count]"r"(outer_loop_cnt),                           \
       [inner_loop_count]"r"(inner_loop_cnt),                           \
       [src_pointer]"r"(src_ptr), [dst_pointer]"r"(dst_ptr)             \
      : "%ymm0", "cc", "memory");



#define SWEEP_FOOTPRINT_x86(OPx86, lc, idx, mem)    \
  asm("mov %[m], %[index]\n\t"                      \
  "loop_start_"#OPx86":"                            \
  OPx86(0x0, index, ymm0)                           \
  OPx86(0x20, index, ymm0)                          \
  OPx86(0x40, index, ymm0)                          \
  OPx86(0x60, index, ymm0)                          \
  OPx86(0x80, index, ymm0)                          \
  OPx86(0xa0, index, ymm0)                          \
  OPx86(0xc0, index, ymm0)                          \
  OPx86(0xe0, index, ymm0)                          \
  OPx86(0x100, index, ymm0)                         \
  OPx86(0x120, index, ymm0)                         \
  OPx86(0x140, index, ymm0)                         \
  OPx86(0x160, index, ymm0)                         \
  OPx86(0x180, index, ymm0)                         \
  OPx86(0x1a0, index, ymm0)                         \
  OPx86(0x1c0, index, ymm0)                         \
  OPx86(0x1e0, index, ymm0)                         \
  OPx86(0x200, index, ymm0)                         \
  OPx86(0x220, index, ymm0)                         \
  OPx86(0x240, index, ymm0)                         \
  OPx86(0x260, index, ymm0)                         \
  OPx86(0x280, index, ymm0)                         \
  OPx86(0x2a0, index, ymm0)                         \
  OPx86(0x2c0, index, ymm0)                         \
  OPx86(0x2e0, index, ymm0)                         \
  OPx86(0x300, index, ymm0)                         \
  OPx86(0x320, index, ymm0)                         \
  OPx86(0x340, index, ymm0)                         \
  OPx86(0x360, index, ymm0)                         \
  OPx86(0x380, index, ymm0)                         \
  OPx86(0x3a0, index, ymm0)                         \
  OPx86(0x3c0, index, ymm0)                         \
  OPx86(0x3e0, index, ymm0)                         \
  OPx86(0x400, index, ymm0)                         \
  OPx86(0x420, index, ymm0)                         \
  OPx86(0x440, index, ymm0)                         \
  OPx86(0x460, index, ymm0)                         \
  OPx86(0x480, index, ymm0)                         \
  OPx86(0x4a0, index, ymm0)                         \
  OPx86(0x4c0, index, ymm0)                         \
  OPx86(0x4e0, index, ymm0)                         \
  OPx86(0x500, index, ymm0)                         \
  OPx86(0x520, index, ymm0)                         \
  OPx86(0x540, index, ymm0)                         \
  OPx86(0x560, index, ymm0)                         \
  OPx86(0x580, index, ymm0)                         \
  OPx86(0x5a0, index, ymm0)                         \
  OPx86(0x5c0, index, ymm0)                         \
  OPx86(0x5e0, index, ymm0)                         \
  OPx86(0x600, index, ymm0)                         \
  OPx86(0x620, index, ymm0)                         \
  OPx86(0x640, index, ymm0)                         \
  OPx86(0x660, index, ymm0)                         \
  OPx86(0x680, index, ymm0)                         \
  OPx86(0x6a0, index, ymm0)                         \
  OPx86(0x6c0, index, ymm0)                         \
  OPx86(0x6e0, index, ymm0)                         \
  OPx86(0x700, index, ymm0)                         \
  OPx86(0x720, index, ymm0)                         \
  OPx86(0x740, index, ymm0)                         \
  OPx86(0x760, index, ymm0)                         \
  OPx86(0x780, index, ymm0)                         \
  OPx86(0x7a0, index, ymm0)                         \
  OPx86(0x7c0, index, ymm0)                         \
  OPx86(0x7e0, index, ymm0)                         \
  "add $0x800, %[index]\n\t"                        \
  "dec %[loop_count]\n\t"                           \
  "jnz loop_start_"#OPx86                           \
  :[loop_count]"+r"(lc), [index]"+r"(idx)           \
  :[m]"m"(mem)                                      \
      :"%ymm0", "cc", "memory");


#define SWEEP_FOOTPRINT_ppc(OPppc, lc, mem)  \
  asm("ld %[reg0], %[m]\n\t"               \
  "addi %[reg1], %[reg0], 0x80\n\t"        \
  "addi %[reg2], %[reg0], 0x100\n\t"       \
  "addi %[reg3], %[reg0], 0x180\n\t"       \
  "li %[reg8], 0x0\n\t"                    \
  "li %[reg9], 0x10\n\t"                   \
  "li %[reg10], 0x20\n\t"                  \
  "li %[reg11], 0x30\n\t"                  \
  "li %[reg12], 0x40\n\t"                  \
  "li %[reg13], 0x50\n\t"                  \
  "li %[reg14], 0x60\n\t"                  \
  "li %[reg15], 0x70\n\t"                  \
  "mtctr %[loop_count]\n\t"                \
  "loop_start_"#OPppc":\n\t"               \
  OPppc(reg0, reg8, v1)                    \
  OPppc(reg0, reg9, v1)                    \
  OPppc(reg0, reg10, v1)                   \
  OPppc(reg0, reg11, v1)                   \
  OPppc(reg0, reg12, v1)                   \
  OPppc(reg0, reg13, v1)                   \
  OPppc(reg0, reg14, v1)                   \
  OPppc(reg0, reg15, v1)                   \
  OPppc(reg1, reg8, v1)                    \
  OPppc(reg1, reg9, v1)                    \
  OPppc(reg1, reg10, v1)                   \
  OPppc(reg1, reg11, v1)                   \
  OPppc(reg1, reg12, v1)                   \
  OPppc(reg1, reg13, v1)                   \
  OPppc(reg1, reg14, v1)                   \
  OPppc(reg1, reg15, v1)                   \
  OPppc(reg2, reg8, v1)                    \
  OPppc(reg2, reg9, v1)                    \
  OPppc(reg2, reg10, v1)                   \
  OPppc(reg2, reg11, v1)                   \
  OPppc(reg2, reg12, v1)                   \
  OPppc(reg2, reg13, v1)                   \
  OPppc(reg2, reg14, v1)                   \
  OPppc(reg2, reg15, v1)                   \
  OPppc(reg3, reg8, v1)                    \
  OPppc(reg3, reg9, v1)                    \
  OPppc(reg3, reg10, v1)                   \
  OPppc(reg3, reg11, v1)                   \
  OPppc(reg3, reg12, v1)                   \
  OPppc(reg3, reg13, v1)                   \
  OPppc(reg3, reg14, v1)                   \
  OPppc(reg3, reg15, v1)                   \
  "addi %[reg0], %[reg0], 0x200\n\t"       \
  "addi %[reg1], %[reg1], 0x200\n\t"       \
  "addi %[reg2], %[reg2], 0x200\n\t"       \
  "addi %[reg3], %[reg3], 0x200\n\t"       \
  "bdnz loop_start_"#OPppc                 \
  :[loop_count]"+r"(lc),                   \
    [reg0]"+r"(reg0), [reg1]"+r"(reg1),    \
    [reg2]"+r"(reg2), [reg3]"+r"(reg3),    \
    [reg8]"+r"(reg8), [reg9]"+r"(reg9),    \
    [reg10]"+r"(reg10), [reg11]"+r"(reg11),\
    [reg12]"+r"(reg12), [reg13]"+r"(reg13),\
    [reg14]"+r"(reg14), [reg15]"+r"(reg15) \
  :[m]"m"(mem)                             \
      :"%v1", "cc", "memory", "ctr");


#define SWEEP_FOOTPRINT_arm(OParm, lc, idx0, idx1, idx2, idx3, post_idx, mem) \
  asm("ldr %[index0], %[m]\n\t"                                               \
      "add %[index1], %[index0], #16\n\t"                                     \
      "add %[index2], %[index0], #32\n\t"                                     \
      "add %[index3], %[index0], #48\n\t"                                     \
      "mov %[post_index], #64\n\t"                                            \
      "loop_start_"#OParm":"                                                  \
      x8(OParm(index0, post_index, v0)                                        \
         OParm(index1, post_index, v0)                                        \
         OParm(index2, post_index, v0)                                        \
         OParm(index3, post_index, v0))                                       \
      "sub %[loop_count], %[loop_count], #1\n\t"                              \
      "cbnz %[loop_count], loop_start_"#OParm                                 \
      :[loop_count]"+r"(lc), [post_index]"+r"(post_idx),                      \
       [index0]"+r"(idx0), [index1]"+r"(idx1),                                \
       [index2]"+r"(idx2), [index3]"+r"(idx3)                                 \
      :[m]"m"(mem)                                                            \
      :"%v0", "cc", "memory");


#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_LOADSTORE_H_
