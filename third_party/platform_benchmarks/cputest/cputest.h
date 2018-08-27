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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_CPUTEST_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_CPUTEST_H_

#include <stdint.h>

struct Result;

struct Result alu_latency();
struct Result shl_reg_latency(const int nonzero);
struct Result shl_imm_latency(const int nonzero);
struct Result cmov_reg_latency();
struct Result mov_elimination();
struct Result max_alu_ipc();

struct Result vector_int_add_latency(const int width);
struct Result max_vector_int_add_bandwidth(const int width);

struct Result max_rep_movs(const int data_size,
                         const unsigned copy_size,
                         const int src_align,
                         const int dst_align,
                         const int df);

struct Result max_avx_copy(const int log_copy_size,
                         const int src_align,
                         const int dst_align,
                         const int force_unaligned_memop);

struct Result max_scalar_load_ipc();
struct Result max_scalar_load_bandwidth();
struct Result max_vector_load_ipc();
struct Result max_vector_load_bandwidth();
struct Result max_vector_loads_at_footprint(const unsigned logbytes);

struct Result max_scalar_store_ipc();
struct Result max_scalar_store_bandwidth();
struct Result max_vector_store_ipc();
struct Result max_vector_store_bandwidth();
struct Result max_vector_stores_at_footprint(const unsigned logbytes);


struct Result branch_history(const uint32_t depth);
struct Result taken_branch_latency();
struct Result indirect_branch_latency();
struct Result btb_capacity(const int logsize);
struct Result btb_conditional_capacity(const int logsize, const int mask);

struct Result stlf_dependent_pointer_chase(const uint32_t chasedepth);
struct Result stlf_independent_pointer_chase(const uint32_t chasedepth);

struct Result rdtsc();
struct Result rdtscp();

struct Test {
  const char* name;
  int    args;
  const char* helpinfo;
  struct Result (*function)();
};

#define NUMTESTS 29

const struct Test alltests[NUMTESTS] = {
  {"--alu_latency",
   0,
   "Dependent ALU latency test (GOPS=back-2-back-add-latency*GHz)",
   alu_latency
  },

  {"--shl_reg_latency Sb",
   1,
   "Dependent SHL r64, r32=Sb test (GOPS=back-2-back-add-latency*GHz)",
   (struct Result (*)())shl_reg_latency
  },

  {"--shl_imm_latency Sb",
   1,
   "Dependent SHL reg, imm=Sb test (Sb=0,1 GOPS=back-2-back-add-latency*GHz)",
   (struct Result (*)())shl_imm_latency
  },

  {"--cmov_reg_latency",
   0,
   "Dependent CMOV reg, reg test (GOPS=back-2-back-cmov-latency*GHz)",
   cmov_reg_latency
  },

  {"--mov_elimination S",
   1,
   "S=1: no unrolling. S=0: Unroll. "
   "Tests if reg reg mov is eliminated (GOPS = elim ? ~GHz : ~(GHz/2))",
   (struct Result (*)())mov_elimination
  },

  {"--max_alu_ipc",
   0,
   "MAX Integer IPC test (GOPS=IPC*GHz)",
   max_alu_ipc},

  {"--vector_int_add_latency W",
   1,
   "Dependent width-W vector int latency test "
   "(GOPS=back-2-back-add-latency*GHz)",
   (struct Result (*)())vector_int_add_latency
  },

  {"--max_vector_int_add_bandwidth W",
   1,
   "MAX width-W vector int IPC test (GOPS=IPC*GHz)",
   (struct Result (*)())max_vector_int_add_bandwidth
  },

  {"--max_rep_movs dsz cpsz sa da df",
   5,
   "dsz: data_size, cpsz: copy_size, "
   "sa: src_align, da: dst_align, df: direction flag",
   (struct Result (*)())max_rep_movs
  },

  {"--maxavxcopy lcpsz sa da fa",
   4,
   "lcpsz: log_copy_size, sa: src_align, da: dst_align, "
   "fa: force unaligned memop",
   (struct Result (*)())max_avx_copy
  },

  {"--max_scalar_load_ipc",
   0,
   "MAX (L1) Scalar Load IPC test (GOPS=IPC*GHz)",
   max_scalar_load_ipc
  },

  {"--max_scalar_load_bandwidth",
   0,
   "MAX (L1) Scalar Load Bandwidth test (GBPS)",
   max_scalar_load_bandwidth
  },

  {"--max_vector_load_ipc",
   0,
   "MAX (L1) Vector Load IPC test (GOPS=IPC*GHz)",
   max_vector_load_ipc
  },

  {"--max_vector_load_bandwidth",
   0,
   "MAX (L1) Vector Load Bandwidth test (GBPS)",
   max_vector_load_bandwidth
  },

  {"--max_vector_loads_at_footprint logb",
   1,
   "MAX Vector Loads for (1<<logb) bytes (Billion vector loads per sec)",
   (struct Result (*)())max_vector_loads_at_footprint
  },

  {"--max_scalar_store_ipc",
   0,
   "MAX (L1) Scalar Store IPC test (GOPS=IPC*GHz)",
   max_scalar_store_ipc
  },

  {"--max_scalar_store_bandwidth",
   0,
   "MAX (L1) Scalar Store Bandwidth test (GBPS)",
   max_scalar_store_bandwidth
  },

  {"--max_vector_store_ipc",
   0,
   "MAX (L1) Vector Store IPC test (GOPS=IPC*GHz)",
   max_vector_store_ipc
  },

  {"--max_vector_store_bandwidth",
   0,
   "MAX (L1) Vector Store Bandwidth test (GBPS)",
   max_vector_store_bandwidth},

  {"--max_vector_stores_at_footprint logb",
   1,
   "MAX Vector Stores for (1<<logb) bytes (Billion vector stores per sec)",
   (struct Result (*)())max_vector_stores_at_footprint
  },

  {"--branch_history N",
   1,
   "Branch History Test (Iter_per_sec = f(history_length))",
   (struct Result (*)())branch_history
  },

  {"--taken_branch_latency",
   0,
   "Test for correctly predicted conditional branch",
   taken_branch_latency
  },

  {"--indirect_branch_latency",
   0,
   "Test for correctly predicted indirect branch",
   indirect_branch_latency
  },

  {"--btbcapacity logsize",
   1,
   "Tests rate of prediction for a logsize unique uncond branches",
   (struct Result (*)())btb_capacity
  },

  {"--btbcondcapacity logsize patternmask",
   2,
   "Tests rate of prediction for a logsize unique conditional branches"
   " with a pattern of taken/not-taken behavior that mimics patternmask",
   (struct Result (*)())btb_conditional_capacity
  },

  {"--rdtsc",
   0,
   "Tests rate of execution of rdtsc (read time stamp counter non serializing)",
   (struct Result (*)())rdtsc
  },

  {"--rdtscp",
   0,
   "Tests rate of execution of rdtscp (read time stamp counter serializing)",
   (struct Result (*)())rdtscp
  },

  {"--stlf_dependent_pointer_chase D",
   1,
   "Tests basic store-to-load forwarding mechanism (dependent load)",
   (struct Result (*)())stlf_dependent_pointer_chase
  },

  {"--stlf_independent_pointer_chase D",
   1,
   "Tests basic store-to-load forwarding mechanism (no dependence)",
   (struct Result (*)())stlf_independent_pointer_chase
  }
};


#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_CPUTEST_CPUTEST_H_
