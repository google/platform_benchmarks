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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_RESULT_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_RESULT_H_

#include <stdint.h>

#define FN_NAME_LENGTH 100
#define METRIC_NAME_LENGTH 100

struct Result {
  char function[FN_NAME_LENGTH];
  int64_t resulthash;
  double metric;
  char metricname[METRIC_NAME_LENGTH];
};

#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_RESULT_H_
