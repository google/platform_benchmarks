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

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "alu.h"
#include "third_party/platform_benchmarks/result.h"
#include "third_party/platform_benchmarks/util.h"

struct Result alu_latency() {
  struct Result result;
  register int64_t a;
  a = 0;
  register int i;
  uint64_t t = now_nsec();
  for (i=0; i < LOOP1M; i++) {
    // 1024 back to back dependent adds per loop iter;
    DEPENDENT_ADDS(x1k, a)
  }
  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
  strcpy(result.function, __FUNCTION__);
  result.resulthash = a;
  strcpy(result.metricname, "GOPS");
  return result;
}

struct Result shl_latency(const int regshift, const int nonzero) {
  struct Result result;
  register int64_t a = 0xdeadbeef;
  register char x;
  register int i;
  uint64_t t = now_nsec();
  switch (regshift) {
    case 0:
      switch (nonzero) {
        case 0:
          for (i=0; i < LOOP1M; i++) {
            // 1024 back to back dependent shl per loop iter;
            DEPENDENT_SHL_IMM(x1k, a, 0)
          }
          break;
        case 1:
          for (i=0; i < LOOP1M; i++) {
            // 1024 back to back dependent shl per loop iter;
            DEPENDENT_SHL_IMM(x1k, a, 1)
          }
          break;
        default:
          assert(nonzero==0 || nonzero==1);
      }
      break;

    case 1:
      x = nonzero;
      for (i=0; i < LOOP1M; i++) {
        // 1024 back to back dependent shl per loop iter;
        DEPENDENT_SHL_REG(x1k, a, x)
      }
      break;
    default:
      assert(regshift==0 || regshift == 1);
  }
  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
  result.resulthash = a;
  strcpy(result.metricname, "GOPS");
  return result;
}

struct Result shl_reg_latency(const int nonzero) {
  struct Result r = shl_latency(1, nonzero);
  sprintf(r.function, "%s(%s)", __FUNCTION__, nonzero?"nonzero":"zero");
  return r;
}

struct Result shl_imm_latency(const int nonzero) {
  struct Result r = shl_latency(0, nonzero);
  sprintf(r.function, "%s(%s)", __FUNCTION__, nonzero?"nonzero":"zero");
  return r;
}

struct Result cmov_reg_latency() {
  struct Result result;
  register int64_t a1, a2;
  a1 = a2 = 0;
  register int i;
  uint64_t t = now_nsec();
  for (i=0; i < LOOP1M; i++) {
    // 512 back to back dependent cmov pairs per loop iter;
    DEPENDENT_CMOV_REG(x512, a1, a2)
  }
  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
  strcpy(result.function, __FUNCTION__);
  result.resulthash = a1^a2;
  strcpy(result.metricname, "GOPS");
  return result;
}

struct Result mov_elimination(const int single_mov_per_iter) {
  struct Result result;
  register int64_t a1=0;
  register int64_t a2=0;
  register int count = single_mov_per_iter? LOOP1G : LOOP1M;
  uint64_t t = now_nsec();
  if (single_mov_per_iter) {
    MOV_ELIMINATION_SINGLE(a1, a2, count)
  }
  else {
    MOV_ELIMINATION_1K(a1, a2, count)
  }
  result.metric = (double)LOOP1G/(now_nsec() - t);
  strcpy(result.function, __FUNCTION__);
  result.resulthash = a1^a2^count;
  strcpy(result.metricname, "GOPS");
  return result;
}

struct Result max_alu_ipc() {

  // Limitations of the test. Test will not find Max ALU latency if ANY of the
  // following happen ...
  // 1. Max ALU IPC > (8 / ADD_latency)
  // 2. If the compiler cannot find 10 integer registers to map ALL if ..
  //    a. The eight variables a0 through a7
  //    b. Inner loop variable i and uint64 a.
  // 3. Decode width < Max ALU IPC. Can't see somebody make a CPU like that.

  struct Result result;
  register uint64_t a;
  register uint64_t a0,a1,a2,a3,a4,a5,a6,a7;
  a = 15;

  uint64_t t = now_nsec();

  a0 = 0; a1 = 0; a2 = 0; a3 = 0;
  a4 = 0; a5 = 0; a6 = 0; a7 = 0;

  register int i;
  for (i=0; i < LOOP16M; i++) {
    // 1024 adds per loop iter to amortize looping costs
    DEPENDENT_ADD_SETS(x128, a, a0, a1, a2, a3, a4, a4, a6, a7)
  }

  result.metric = (double)LOOP16M*LOOP1K/(now_nsec()-t);
  strcpy(result.metricname, "GOPS");
  strcpy(result.function, __FUNCTION__);
  result.resulthash = a0+a1+a2+a3+a4+a5+a6+a7;
  return result;
}
