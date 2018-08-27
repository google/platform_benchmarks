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

#include <inttypes.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "third_party/platform_benchmarks/result.h"
#include "third_party/platform_benchmarks/util.h"
#include "branch.h"

struct Result taken_branch_latency() {
  struct Result result;
  register uint64_t loop_count = LOOP1G;
  uint64_t t = now_nsec();

  TAKEN_BRANCH_LOOP(loop_count)

  result.metric = (double)LOOP1G/(now_nsec()-t);
  strcpy(result.metricname, "GOPS");
  strcpy(result.function, __FUNCTION__);
  result.resulthash = loop_count;
  return result;
}

struct Result indirect_branch_latency() {
  struct Result result;
  register uint64_t loop_count = LOOP1G;
  register uint64_t target_branch = 0;
  register uint64_t start_reg = 0;
  register uint64_t mid_reg = 0;
  register uint64_t exit_reg = 0;

  uint64_t t = now_nsec();

  REG_INDIRECT_BRANCH_LOOP(loop_count, target_branch, start_reg, mid_reg, exit_reg)

  result.metric = (double)LOOP1G/(now_nsec()-t);
  strcpy(result.metricname, "GOPS");
  strcpy(result.function, __FUNCTION__);
  result.resulthash = (loop_count + target_branch +
                       start_reg + mid_reg + exit_reg);
  return result;
}

struct Result branch_history(const uint32_t depth) {

  struct Result result;
  uint64_t branchdepth = (1u<<(31-__builtin_clz(depth))) - 1;

  srand(RAND_SEED);
  uint64_t* c = (uint64_t *) malloc(sizeof(uint64_t) * (branchdepth + 1));

  register uint64_t i;
  for (i=0; i <= branchdepth; i++) {
    c[i] = rand() & 0x1;
  }
  register uint64_t x = 0;

  uint64_t loop_count = LOOP16M;
  uint64_t t = now_nsec();

  BRANCH_HISTORY(loop_count, i, branchdepth, c, x)

  result.metric = (double)LOOP16M/(now_nsec()-t);
  strcpy(result.metricname, "Billion_Iter_per_sec");
  snprintf(result.function, FN_NAME_LENGTH, "%s(%lu)", __FUNCTION__, branchdepth + 1);
  result.resulthash = x;
  return result;
}

struct Result btb_capacity(const int logsize) {
  struct Result result;
  register uint64_t loop_count = (uint64_t)1 << (30 - logsize);
  uint64_t t = now_nsec();

  BRANCH_SWEEP(UNCOND_DIRECT_BRANCHES, logsize, loop_count, 0);

  result.metric = (double)(LOOP1G + loop_count)/(now_nsec()-t);
  strcpy(result.metricname, "Billion_Branches_per_sec");
  sprintf(result.function, "%s(logsize=%d)", __FUNCTION__, logsize);
  result.resulthash = loop_count;
  return result;
}

struct Result btb_conditional_capacity(const int logsize, const int mask) {
  struct Result result;
  register uint64_t loop_count = (uint64_t)1 << (30 - logsize);
  register uint64_t pattern_mask = mask;
  uint64_t t = now_nsec();

  BRANCH_SWEEP(COND_BRANCHES, logsize, loop_count, pattern_mask)

  result.metric = (double)(LOOP1G + loop_count)/(now_nsec()-t);
  strcpy(result.metricname, "Billion_Branches_per_sec");
  sprintf(result.function, "%s(logsize=%d,patternmask=%" PRIx64 ")", __FUNCTION__,
          logsize, pattern_mask);
  result.resulthash = loop_count;
  return result;
}
