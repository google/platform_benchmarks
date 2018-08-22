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

#include "third_party/platform_benchmarks/result.h"
#include "third_party/platform_benchmarks/util.h"
#include "vector.h"

struct Result vector_int_add_latency(const int width) {
  struct Result result;
  register int i;
  uint64_t t = now_nsec();
  for (i=0; i < LOOP1M; i++) {
    // 1024 back to back dependent vector adds per loop iter;
    DEPENDENT_VECTOR256_INT_ADDS(x1k);
  }
  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
  sprintf(result.function, "%s(width=%d)", __FUNCTION__, width);
  result.resulthash = i;
  strcpy(result.metricname, "GOPS");
  return result;
}


struct Result max_vector_int_add_bandwidth(const int width) {

// Limitations of the test. Test will not find Max Vector Add Throughput
// if ANY of the following happen ...
// 1. Max VectorADD IPC > (8 / Vector_ADD_latency)
// 2. Decode width < Max Vector Int Add IPC. Unlikely.

  struct Result result;
  assert(width==256); // this is a temp assert.
  uint64_t t = now_nsec();

  register int i;
  for (i=0; i < LOOP16M; i++) {
    // 1024 adds per loop iter to amortize looping costs
    DEPENDENT_VECTOR256_INT_ADD_SETS(x128)
  }

  result.metric = (double)LOOP16M*LOOP1K/(now_nsec()-t);
  strcpy(result.metricname, "GOPS");
  sprintf(result.function, "%s(width=%d)", __FUNCTION__, width);
  result.resulthash = i;
  return result;
}
