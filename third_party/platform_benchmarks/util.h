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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_UTIL_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_UTIL_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define x1(x) x
#define x2(x) x x
#define x4(x) x2(x) x2(x)
#define x8(x) x4(x) x4(x)
#define x16(x) x8(x) x8(x)
#define x32(x) x16(x) x16(x)
#define x64(x) x32(x) x32(x)
#define x128(x) x64(x) x64(x)
#define x256(x) x128(x) x128(x)
#define x512(x) x256(x) x256(x)
#define x1k(x) x512(x) x512(x)
#define x2k(x) x1k(x) x1k(x)
#define x4k(x) x2k(x) x2k(x)
#define x8k(x) x4k(x) x4k(x)
#define x16k(x) x8k(x) x8k(x)
#define x32k(x) x16k(x) x16k(x)
#define x64k(x) x32k(x) x32k(x)
#define x128k(x) x64k(x) x64k(x)
#define x256k(x) x128k(x) x128k(x)

#define LOOP64 (1<<6)    // Use for repeating 64 times
#define LOOP1K (1<<10)   // Use for repeating 1024 times.
#define LOOP4K (1<<12)   // Use for repeating 4096 times.
#define LOOP1M (1<<20)   // Use for repeating 2^20 times
#define LOOP2M (1<<21)   // Use for repeating 2^21 times
#define LOOP4M (1<<22)   // Use for repeating 2^22 times
#define LOOP16M (1<<24)  // Use for repeating 2^24 times
#define LOOP64M (1<<26)  // Use for repeating 2^26 times
#define LOOP128M (1<<27)  // Use for repeating 2^27 times
#define LOOP1G (1lu << 30)  // Use for repeating 2^30 times
#define LOOP16G (1lu << 34)  // Use for repeating 2^34 times

#define MAX_CACHELINE_SIZE 0x80  // 128-byte cacheline max.

#define MAX_BYTE_STRING_LENGTH 100

#define RAND_BUF_SIZE 32

static inline uint64_t now_nsec(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * ((uint64_t)1000*1000*1000) + ts.tv_nsec;
}

void byte2string(char* str, const unsigned logbytes);
void* randmemset(void *s, size_t n, unsigned randseed);

#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_UTIL_H_
