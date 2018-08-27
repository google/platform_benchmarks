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

#ifndef PLATFORMS_BENCHMARKS_MICROBENCHMARKS_ANALOGS_EXEGESIS_MEMCPY_H_
#define PLATFORMS_BENCHMARKS_MICROBENCHMARKS_ANALOGS_EXEGESIS_MEMCPY_H_

#include <string.h>

// Copy block_size bytes from source to destination.
static void exegesis_copyblock(char* dst, const char* src, size_t block_size) {
  __builtin_memcpy(dst, src, block_size);
}

// On x86_64 copying twice with an overlap is quite fast.
// e.g.
// [--------------] 14 bytes are copied in two steps:
// [--------      ] First copy 8 bytes from the start,
// [      --------] Then copy 8 bytes from the end.
static void exegesis_overlap(
    char* dst, const char* src, size_t count, size_t block_size) {
#ifndef NDEBUG
  if (count < block_size || count > block_size * 2) {
    abort();
  }
#endif
  exegesis_copyblock(dst, src, block_size);

  const size_t offset = count - block_size;
  exegesis_copyblock(dst + offset, src + offset, block_size);
}

// Uses the x86 "rep movs" instruction.
static void CopyRepMovsb(char* dst, const char* src, size_t count) {
  asm volatile("rep movsb" : "+c"(count), "+S"(src), "+D"(dst) : : "memory");
}

#define DO_AND_RETURN_IF(COND, X, BS) \
  if (COND) { X(dst, src, count, BS); return; }

static void Copy_V1(char* const dst, const char* const src, size_t count) {
  if (count == 0) return;
  if (count == 1) return exegesis_copyblock(dst, src, 1);
  if (count == 2) return exegesis_copyblock(dst, src, 2);
  if (count == 3) return exegesis_copyblock(dst, src, 3);
  if (count == 4) return exegesis_copyblock(dst, src, 4);
  DO_AND_RETURN_IF(count <= 8, exegesis_overlap, 4);
  DO_AND_RETURN_IF(count <= 16, exegesis_overlap, 8);
  DO_AND_RETURN_IF(count <= 32, exegesis_overlap, 16);
  DO_AND_RETURN_IF(count <= 64, exegesis_overlap, 32);
  DO_AND_RETURN_IF(count <= 128, exegesis_overlap, 64);
  CopyRepMovsb(dst, src, count);
}

static inline __attribute__((flatten)) void *exegesis_memcpy(
    void *dst, const void *src, size_t count) {
  Copy_V1((char*)(dst), (const char*)(src), count);
  return dst;
}

#endif  // PLATFORMS_BENCHMARKS_MICROBENCHMARKS_ANALOGS_EXEGESIS_MEMCPY_H_
