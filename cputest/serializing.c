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

#include "third_party/platform_benchmarks/result.h"
#include "third_party/platform_benchmarks/util.h"

struct Result rdtsc() {
  struct Result result;
  uint64_t t = now_nsec();
#ifdef __x86_64__
  for (int i=0; i < LOOP1M; i++) {
    asm(x1k("rdtsc\n\t"):::"%rdx","%rax");
  }
#endif
  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
#ifdef __x86_64__
  strcpy(result.function, __FUNCTION__);
#else
  sprintf(result.function, "%s NOT APPLICABLE on Current Platform", __FUNCTION__);
#endif
  result.resulthash = t;  // not meaningful
  strcpy(result.metricname, "Billion_rdtsc_per_sec");
  return result;
}

struct Result rdtscp() {
  struct Result result;
  uint64_t t = now_nsec();
#ifdef __x86_64__
  for (int i=0; i < LOOP1M; i++) {
    asm(x1k("rdtsc\n\t"):::"%rdx","%rax");
  }
#endif
  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
#ifdef __x86_64__
  strcpy(result.function, __FUNCTION__);
#else
  sprintf(result.function, "%s NOT APPLICABLE on Current Platform", __FUNCTION__);
#endif
  result.resulthash = t;  // not meaningful
  strcpy(result.metricname, "Billion_rdtscp_per_sec");
  return result;
}
