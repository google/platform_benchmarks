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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "third_party/platform_benchmarks/result.h"
#include "third_party/platform_benchmarks/util.h"
#include "store.h"

struct Result max_scalar_memop_ipc(const unsigned load);
struct Result max_vector_memop_ipc(const unsigned load);
struct Result max_vector_memops_at_footprint(const unsigned load, const unsigned logbytes);


struct Result max_scalar_store_ipc() {
  struct Result result = max_scalar_memop_ipc(STORE);
  strcpy(result.function, __FUNCTION__);
  return result;
}

struct Result max_scalar_store_bandwidth() {
  struct Result result = max_scalar_store_ipc();
  result.metric = result.metric * SCALAR_STORE_WIDTH;
  strcpy(result.metricname, "GB/s");
  strcpy(result.function, __FUNCTION__);
  return result;
}

struct Result max_vector_store_ipc() {
  struct Result result = max_vector_memop_ipc(STORE);
  strcpy(result.function, __FUNCTION__);
  return result;
}


struct Result max_vector_store_bandwidth() {
  struct Result result = max_vector_store_ipc();
  result.metric = result.metric * VECTOR_STORE_WIDTH;
  strcpy(result.metricname, "GB/s");
  strcpy(result.function, __FUNCTION__);
  return result;
}

struct Result max_vector_stores_at_footprint(const unsigned logbytes) {
  struct Result result;
  result = max_vector_memops_at_footprint(STORE, logbytes);
  char str[MAX_BYTE_STRING_LENGTH];
  byte2string(str, logbytes);
  snprintf(result.function, FN_NAME_LENGTH, "%s(%s)", __FUNCTION__, str);
  return result;
}
