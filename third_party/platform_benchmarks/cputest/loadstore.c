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

#include "load.h"
#include "loadstore.h"
#include "third_party/platform_benchmarks/result.h"
#include "third_party/platform_benchmarks/util.h"
#include "store.h"

#define MAXCHASEDEPTH 32

#define DEPENDENT 1
#define INDEPENDENT 0


struct Result stlf_pointer_chase(const int dependent, const uint32_t normalizeddepth) {
  struct Result result;
  uint64_t* m = (uint64_t *)malloc(sizeof(uint64_t) * (MAXCHASEDEPTH + 1));

  register uint64_t loopcount;

  // set up pointers correctly
  for (loopcount=0; loopcount < (MAXCHASEDEPTH+1); loopcount++) {
    m[loopcount] = (uint64_t)(&(m[loopcount+1]));
  }

  uint64_t t;
  register uint64_t loadregister = 0;
  register uint64_t startp = (uint64_t)m;
  register uint64_t p;
  register uint64_t readlocation = (uint64_t)(dependent?(m+normalizeddepth):m);
  loopcount = LOOP1M;

  // forwarding case
  t = now_nsec();

  switch (normalizeddepth) {
    case 2:
      STLF_LOOP_CHASE(loopcount, 2, readlocation, startp, p, loadregister);
      break;
    case 4:
      STLF_LOOP_CHASE(loopcount, 4, readlocation, startp, p, loadregister);
      break;
    case 8:
      STLF_LOOP_CHASE(loopcount, 8, readlocation, startp, p, loadregister);
      break;
    case 16:
      STLF_LOOP_CHASE(loopcount, 16, readlocation, startp, p, loadregister);
      break;
    case 32:
      STLF_LOOP_CHASE(loopcount, 32, readlocation, startp, p, loadregister);
      break;
    default:
      STLF_LOOP_CHASE(loopcount, 2, readlocation, startp, p, loadregister);
      break;
  }

  result.metric = (double)LOOP1M/(now_nsec()-t);
  result.resulthash = loadregister;
  strcpy(result.metricname, "Billion_Iter_per_sec");
  return result;
}

struct Result stlf_dependent_pointer_chase(const uint32_t chasedepth) {
  uint32_t normalizeddepth = (1u<<(31-__builtin_clz(chasedepth)));
  normalizeddepth = (normalizeddepth > 32 ? 32 : ((normalizeddepth < 2)? 2 : normalizeddepth));

  struct Result r = stlf_pointer_chase(DEPENDENT,normalizeddepth);
  snprintf(r.function, FN_NAME_LENGTH, "%s(%d)", __FUNCTION__, normalizeddepth);
  return r;
}

struct Result stlf_independent_pointer_chase(const uint32_t chasedepth) {
  uint32_t normalizeddepth = (1u<<(31-__builtin_clz(chasedepth)));
  normalizeddepth = (normalizeddepth > 32 ? 32 : ((normalizeddepth < 2)? 2 : normalizeddepth));

  struct Result r = stlf_pointer_chase(INDEPENDENT,normalizeddepth);
  snprintf(r.function, FN_NAME_LENGTH, "%s(%d)", __FUNCTION__, normalizeddepth);
  return r;
}


struct Result max_scalar_memop_ipc(const unsigned load) {

  // Limitations of the test. Test will not find Max Load latency if ANY of the
  // following happen ...
  // 1. Decode width < Max Load IPC. Can't see somebody make a CPU like that.

  struct Result result;
  uint64_t* m = (uint64_t *)malloc(sizeof(uint64_t));
  register int i;
  uint64_t t = now_nsec();

  if (load) {
    for (i=0; i < LOOP1M; i++) {
      // translates to 1024 loads per loop iter.
      SCALAR_LOADS(x1k, m)
    }
  }
  else {
    for (i=0; i < LOOP1M; i++) {
      // translates to 1024 loads per loop iter.
      SCALAR_STORES(x1k, m)
    }
  }

  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
  strcpy(result.metricname, "GOPS");
  result.resulthash = i;
  return result;
}


uint64_t allocate_for_copy(const unsigned copy_size,
                           const int cacheline_size,
                           void **memref) {
  // returns outer loop count for test.
  // Returns 0 if cannot allocate.

  assert(copy_size <= (1u << MAX_LOG_COPYSIZE));

  // 2x 2^log_copy_size + separation between source and dest cachelines
  // such that there's no overlap.

  // Worst case example:
  // 64 byte copy from src_align=7 to dst_align=7
  // Src range: 0007 - 0046
  // Dst range: 0087 - 00c6
  // End position of dest = 00c7

  uint64_t allocsize = 2 * copy_size + 2 * cacheline_size;

  if (posix_memalign(memref, cacheline_size, allocsize) != 0) {
    perror("Unable to align to cacheline size\n");
    return 0llu;
  }

  // we don't copy tests to run forever.
  // we also don't want to have it run too quickly because
  // now_nsec() might give us a less-than-accurate answer.
  // let's aim to have a max of 1 cold traversal of
  // largest potential LLC per 100 traversals.
  // let's aim to have a min of 1 sec of total runtime
  // Copy bandwidth can be unpredictably low at small sizes.
  // Below table is the result of some prelim measurement.


  const uint64_t outer_loop_count_table[MAX_LOG_COPYSIZE + 1] =
      {1llu << 27,  // 128M x 1B @ ~1 GB/s = 1/8 sec (to guard against bad perf)
       1llu << 27,  // 128M x 2B @ ~1 GB/s = 1/4 sec (to guard against bad perf)
       1llu << 27,  // 128M x 4B @ ~1 GB/s = 1/2 sec (to guard against bad perf)
       1llu << 27,  // 128M x 8B @ ~1 GB/s = ~1 sec
       1llu << 27,  // 128M x 16B @ , ~2 GB/s = ~1 sec
       1llu << 27,  // 128M x 32B @ ~4 GB/s = ~1 sec
       1llu << 27,  // 128M x x64B @ ~8 GB/s = ~1 sec
       1llu << 26,  // 64M x 128B @ ~8 GB/s = ~1 sec
       1llu << 26,  // 64M x 256B @ ~16 GB/s = ~1 sec
       1llu << 26,  // 64M x 512B @ ~32 GB/s = ~1 sec
       1llu << 25,  // 32M x 1KB @ ~32 GB/s = ~1 sec
       1llu << 25,  // 32M x 2KB @ ~64 GB/s = ~1 sec
       1llu << 24,  // 16M x 4KB @ ~64 GB/s = ~1 sec
       1llu << 23,  // 8M x 8 KB @ ~64 GB/s = ~1 sec
       1llu << 22,  // 4M x 16KB @ ~64 GB/s = ~1 sec
       1llu << 20,  // 1M x 32KB  @ ~32 GB/s = ~1 sec
       1llu << 19,  // 512K x 64KB @ ~32 GB/s = ~1 sec
       1llu << 18,  // 256K x 128KB @ ~32 GB/s = ~1 sec
       1llu << 17,  // 128K x 256KB @ ~32 GB/s = ~1 sec
       1llu << 16,  // 64K x 512KB @ ~32 GB/s = ~1 sec
       1llu << 14,  // 16K x 1MB @ ~16 GB/s = ~1 sec
       1llu << 12,  // 4K x 2MB @ ~8 GB/s = ~1 sec
       1llu << 11,  // 2K x 4MB @ ~8 GB/s = ~1 sec
       1llu << 10,  // 1K x 8MB @ ~8 GB/s = ~1 sec
       1llu << 9,   // 512 x 16MB @ ~8 GB/s = ~1 sec
       1llu << 8,   // 256 x 32MB @ <8 GB/s = > 1 sec
       1llu << 7,   // 128 x 64MB @ <8 GB/s = > 1 sec
       1llu << 7,   // 128 x 128MB @ <8 GB/s = > 2 sec (to warm up L3 upto 128MB)
       1llu << 6,   // 64 x 256MB @ <8 GB/s = > 2 sec (to warm up L3 upto 128MB)
       1llu << 5,   // 32 x 512MB @ <8 GB/s = > 2 sec (to warm up L3 upto 128MB)
       1llu << 4,   // 16 x 1GB @ <8 GB/s = > 2 sec (to warm up L3 upto 128MB)
       1llu << 3,   // 8 x 2GB @ <8 GB/s = > 2 sec (to warm up L3 upto 128MB)
      };

  return outer_loop_count_table[copy_size == 0 ? 0 : 31 - __builtin_clz(copy_size)];
}


struct Result max_rep_movs(const int data_size,
                         const unsigned copy_size,
                         const int src_align,
                         const int dst_align,
                         const int df) {

  struct Result result;

#ifndef __x86_64__
  sprintf(result.function, "%s NOT APPLICABLE on Current Platform", __FUNCTION__);
#else

  // Clean multiple of data size
  assert((copy_size & (data_size - 1)) == 0);

  char* m;
  const int cacheline_size = 64;
  register uint64_t outer_loop_count = allocate_for_copy(copy_size,
                                                         cacheline_size,
                                                         (void **)&m);
  if (outer_loop_count == 0) {
    sprintf(result.function, "%s could not allocate memory", __FUNCTION__);
  }

  register uint64_t copy_count = (copy_size / data_size);

  register uint64_t src_pointer = ((uint64_t)m + src_align +
                                   (df == 0?
                                    0 :
                                    (copy_size - data_size)));

  // dst_pointer starts on a cacheline separate from src copy range.
  register uint64_t dst_pointer = ((uint64_t)m +
                                   ((src_align + copy_size) /
                                    cacheline_size
                                    + 1) *
                                   cacheline_size +
                                   dst_align +
                                   (df == 0?
                                    0 :
                                    (copy_size - data_size)));

  register uint64_t loop_counter = outer_loop_count;

  // data_size = 1,2,4 or 8
  assert(data_size >= 1 && data_size <= 8 &&
         (data_size & (data_size - 1)) == 0 &&
         (df == 0 || df == 1));

  uint64_t t = now_nsec();

  switch ((df << 4) | data_size) {

    // These are 3 independent register moves
    // outer_loop_count is a register.
    // std and cld are each single rflags RMW operations

    // 3 reg moves, max 2 flags moves, 1 reg add,
    // dependent reg cmp, and branch
    // est overhead <= 3 cycles.

    case 1:
      {
        // byte upwards
        REP_MOVS_LOOP(loop_counter,
                      copy_count, src_pointer, dst_pointer,
                      "cld\n\t","rep movsb\n\t","\n")
            break;
      }
    case 2:
      {
        // word upwards
        REP_MOVS_LOOP(loop_counter,
                      copy_count, src_pointer, dst_pointer,
                      "cld\n\t", "rep movsw\n\t", "\n")
        break;
      }
    case 4:
      {
        // dword upwards
        REP_MOVS_LOOP(loop_counter,
                      copy_count, src_pointer, dst_pointer,
                      "cld\n\t", "rep movsd\n\t", "\n")
        break;
      }
    case 8:
      {
        // qword upwards
        REP_MOVS_LOOP(loop_counter,
                      copy_count, src_pointer, dst_pointer,
                      "cld\n\t", "rep movsq\n\t", "\n")
        break;
      }
    case 17:
      {
        // byte downwards
        REP_MOVS_LOOP(loop_counter,
                      copy_count, src_pointer, dst_pointer,
                      "std\n\t", "rep movsb\n\t", "cld\n")
        break;
      }
    case 18:
      {
        // word downwards
        REP_MOVS_LOOP(loop_counter,
                      copy_count, src_pointer, dst_pointer,
                      "std\n\t", "rep movsw\n\t", "cld\n")
        break;
      }
    case 20:
      {
        // dword downwards
        REP_MOVS_LOOP(loop_counter,
                      copy_count, src_pointer, dst_pointer,
                      "std\n\t", "rep movsd\n\t", "cld\n")
        break;
      }
    case 24:
      {
        // qword downwards
        REP_MOVS_LOOP(loop_counter,
                      copy_count, src_pointer, dst_pointer,
                      "std\n\t", "rep movsq\n\t", "cld\n")
        break;
      }
    default:
      // can do only 1,2,4 or 8 byte rep mov operations.
      // and df can only be 0 or 1
      assert(0);
  }

  result.metric = ((double)outer_loop_count *
                   copy_size / (now_nsec() - t));
  result.resulthash = (src_pointer << 32) |
                      (dst_pointer & (((uint64_t)1 << 32) - 1));
  strcpy(result.metricname, "GBPS");


  sprintf(result.function,
          "%s(dsize: %d, cpsize: %d,\tsrcalign: %d, dstalign: %d, df: %d)",
          __FUNCTION__, data_size, copy_size,
          src_align, dst_align, df);
#endif

  return result;
}

#ifdef __x86_64__
void avx_default_copy_loop(const register uint64_t outer_loop_count,
                           const int log_copy_size,
                           register uint64_t src_pointer,
                           register uint64_t dst_pointer,
                           const int src_align,
                           const int dst_align,
                           const int force_unaligned_memop) {

  assert (log_copy_size > 8);

  register uint64_t outer_loop_index_reg;
  register uint64_t src_pointer_reg;
  register uint64_t dst_pointer_reg;
  register uint64_t inner_loop_count = (1 << (log_copy_size - 8));
  register uint64_t inner_loop_index_reg = inner_loop_count;

  int switch_key =  ((((src_align & 0x1f) != 0 || force_unaligned_memop) ? 0 : 2) |
                     (((dst_align & 0x1f) != 0 || force_unaligned_memop) ? 0 : 1));

  switch (switch_key) {
    case 0:
      AVX_COPY_LOOP(outer_loop_index_reg, outer_loop_count,
                    inner_loop_index_reg, inner_loop_count,
                    src_pointer_reg, src_pointer,
                    dst_pointer_reg, dst_pointer,
                    VMOVUPD_LOAD, VMOVUPD_STORE)
      break;

    case 1:
      AVX_COPY_LOOP(outer_loop_index_reg, outer_loop_count,
                    inner_loop_index_reg, inner_loop_count,
                    src_pointer_reg, src_pointer,
                    dst_pointer_reg, dst_pointer,
                    VMOVUPD_LOAD, VMOVAPD_STORE)
      break;

    case 2:
      AVX_COPY_LOOP(outer_loop_index_reg, outer_loop_count,
                    inner_loop_index_reg, inner_loop_count,
                    src_pointer_reg, src_pointer,
                    dst_pointer_reg, dst_pointer,
                    VMOVAPD_LOAD, VMOVUPD_STORE)
      break;

    case 3:
      AVX_COPY_LOOP(outer_loop_index_reg, outer_loop_count,
                    inner_loop_index_reg, inner_loop_count,
                    src_pointer_reg, src_pointer,
                    dst_pointer_reg, dst_pointer,
                    VMOVAPD_LOAD, VMOVAPD_STORE)
      break;

    default:
      assert(0);
  };
}
#endif

struct Result max_avx_copy(const int log_copy_size,
                         const int src_align,
                         const int dst_align,
                         const int force_unaligned_memop) {
  struct Result result;

#ifndef __x86_64__
  sprintf(result.function, "%s NOT APPLICABLE on Current Platform", __FUNCTION__);
#else

  char* m;
  const int cacheline_size = 64;

  assert(log_copy_size >= 5); // Make sure avx works with this

  register uint64_t outer_loop_count = allocate_for_copy(1 << log_copy_size,
                                                         cacheline_size,
                                                         (void **)&m);

  if (outer_loop_count == 0) {
    sprintf(result.function, "%s could not allocate memory", __FUNCTION__);
    return result;
  }

  register uint64_t src_pointer = (uint64_t)m + src_align;
  register uint64_t dst_pointer = ((uint64_t)m +
                                   ((src_align + (1 << log_copy_size)) /
                                    cacheline_size +
                                    1) *
                                   cacheline_size
                                   + dst_align);

  int switch_key = ((log_copy_size << 2) |
                    (((src_align & 0x1f) != 0 || force_unaligned_memop) ? 0 : 2) |
                    (((dst_align & 0x1f) != 0 || force_unaligned_memop) ? 0 : 1));

  register uint64_t loop_counter = outer_loop_count;
  uint64_t t = now_nsec();

  switch (switch_key) {
    case (5 << 2) | 0:
      AVX_32BYTE_COPY_LOOP(loop_counter,
                           VMOVUPD_LOAD, VMOVUPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (5 << 2) | 1:
      AVX_32BYTE_COPY_LOOP(loop_counter,
                           VMOVUPD_LOAD, VMOVAPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (5 << 2) | 2:
      AVX_32BYTE_COPY_LOOP(loop_counter,
                           VMOVAPD_LOAD, VMOVUPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (5 << 2) | 3:
      AVX_32BYTE_COPY_LOOP(loop_counter,
                           VMOVAPD_LOAD, VMOVAPD_STORE,
                           src_pointer, dst_pointer)
      break;

    case (6 << 2) | 0:
      AVX_64BYTE_COPY_LOOP(loop_counter,
                           VMOVUPD_LOAD, VMOVUPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (6 << 2) | 1:
      AVX_64BYTE_COPY_LOOP(loop_counter,
                           VMOVUPD_LOAD, VMOVAPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (6 << 2) | 2:
      AVX_64BYTE_COPY_LOOP(loop_counter,
                           VMOVAPD_LOAD, VMOVUPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (6 << 2) | 3:
      AVX_64BYTE_COPY_LOOP(loop_counter,
                           VMOVAPD_LOAD, VMOVAPD_STORE,
                           src_pointer, dst_pointer)
      break;

    case (7 << 2) | 0:
      AVX_128BYTE_COPY_LOOP(loop_counter,
                           VMOVUPD_LOAD, VMOVUPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (7 << 2) | 1:
      AVX_128BYTE_COPY_LOOP(loop_counter,
                           VMOVUPD_LOAD, VMOVAPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (7 << 2) | 2:
      AVX_128BYTE_COPY_LOOP(loop_counter,
                           VMOVAPD_LOAD, VMOVUPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (7 << 2) | 3:
      AVX_128BYTE_COPY_LOOP(loop_counter,
                           VMOVAPD_LOAD, VMOVAPD_STORE,
                           src_pointer, dst_pointer)
      break;

    case (8 << 2) | 0:
      AVX_256BYTE_COPY_LOOP(loop_counter,
                           VMOVUPD_LOAD, VMOVUPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (8 << 2) | 1:
      AVX_256BYTE_COPY_LOOP(loop_counter,
                           VMOVUPD_LOAD, VMOVAPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (8 << 2) | 2:
      AVX_256BYTE_COPY_LOOP(loop_counter,
                           VMOVAPD_LOAD, VMOVUPD_STORE,
                           src_pointer, dst_pointer)
      break;
    case (8 << 2) | 3:
      AVX_256BYTE_COPY_LOOP(loop_counter,
                           VMOVAPD_LOAD, VMOVAPD_STORE,
                           src_pointer, dst_pointer)
      break;


    default:
      // these are cases where we are willing to tolerate
      // an initial inner loop mispredict because it will be
      // amortized over a longer amount of time.
      avx_default_copy_loop(outer_loop_count,
                            log_copy_size,
                            src_pointer,
                            dst_pointer,
                            src_align,
                            dst_align,
                            force_unaligned_memop);
  }

  result.metric = ((double)outer_loop_count *
                   (1 << log_copy_size) / (now_nsec() - t));
  result.resulthash = (src_pointer << 32) |
      (dst_pointer & (((uint64_t)1 << 32) - 1));
  strcpy(result.metricname, "GBPS");

  char bytesstring[20];
  byte2string(bytesstring, log_copy_size);

  sprintf(result.function,
          "%s(cpsize: %s,\tsrcalign: %d, dstalign: %d)",
          __FUNCTION__, bytesstring, src_align, dst_align);
#endif

  return result;
}


struct Result max_vector_memop_ipc(const unsigned load) {
  struct Result result;

  double* memdouble;
  if (posix_memalign((void **)&memdouble,
                     load?VECTOR_LOAD_WIDTH:VECTOR_STORE_WIDTH,
                     sizeof(double)*8) != 0) {
    perror("Unable to align to vector width\n");
    abort();
  }

  register int i;
  uint64_t t = now_nsec();

  if (load) {
    for (i=0; i < LOOP1M; i++) {
      VECTOR_LOADS(x1k, memdouble)
    }
  }
  else {
    for (i=0; i < LOOP1M; i++) {
      VECTOR_STORES(x1k, memdouble)
    }
  }

  result.metric = (double)LOOP1M*LOOP1K/(now_nsec()-t);
  strcpy(result.metricname, "GOPS");
  strcpy(result.function, __FUNCTION__);
  result.resulthash = (int)memdouble[0];
  return result;
}

struct Result max_vector_memops_at_footprint(const unsigned load, const unsigned logbytes) {
  struct Result result;

  // If this is not assuredly cacheline aligned spillover effects may
  // completely compromise replacement policy even if footprint is
  // contained in the cache.
  uint64_t* m;
  if (posix_memalign((void **)&m, MAX_CACHELINE_SIZE, (1<<logbytes))!=0) {
    perror("Unable to align to cacheline size\n");
    abort();
  }

  // need randmemset to maintain realistic scenario under ...
  // a. lazy allocation
  // b. compression for higher bandwidth
  // Doesn't seem to be necessary with GRTE but need it for other environments
  randmemset(m, 1<<logbytes, RAND_SEED);

  register uint64_t i;
  register uint64_t loop_count;

  // For under 2K bytes, at max vector bandwidth, not enough room to unroll loop
  // so that branch penalties, if any, can be amortized.
  assert(logbytes >= 11);

  // Don't plan to have this go above 16GB
  assert(logbytes <= 34);

  // I don't want this test to run forever, so will limit loop_count.
  // If we assume 256MB as the upper limit of L3 cache size, you'd still
  // loop through 256 times -- enough to amortize the cost of cold misses.
  uint64_t outerloopcount = logbytes > 16 ? (LOOP1M >> (logbytes - 16)): LOOP1M;

#ifdef __x86_64__

  // x86 can do 32-bytes per load. To cover bytes you need...
  // bytes / 32 = bytes >> 5 = (1 << (logbytes - 5)) loads

  // Each iteration of loop_start_max_vector_loads_at_footprint in the inline assembly
  // will do 64 loads.
  // loop_count  = (1<< (logbytes-5))/64 = (1 << (logbytes-11))


  register uint64_t index;

  uint64_t t = now_nsec();
  if (load) {
    for (i=0; i < outerloopcount; i++) {
      loop_count = (1<<(logbytes-11));
      SWEEP_FOOTPRINT_x86(VECTORLOADx86, loop_count, index, m)
    }
  }
  else {
    for (i=0; i < outerloopcount; i++) {
      loop_count = (1<<(logbytes-11));
      SWEEP_FOOTPRINT_x86(VECTORSTOREx86, loop_count, index, m)
    }
  }
  result.resulthash = outerloopcount;
  strcpy(result.metricname, load? "Billion_32_byte_loads_per_sec" : "Billion_32_byte_stores_per_sec");
  result.metric = (double)outerloopcount*(1<<(logbytes-5))/(now_nsec()-t);
#endif

#ifdef __PPC64__

  // PPC can do 16-bytes per load. To cover bytes you need...
  // bytes / 16 = bytes >> 4 = (1 << (logbytes - 4)) loads

  // Each iteration of loop_start_max_vector_loads_at_footprint in the inline assembly
  // will do 32 loads.
  // loop_count  = (1<< (logbytes-4))/32 = (1 << (logbytes-9))

  // Need to explicitly map to registers because
  // assembler messes up if I don't!

  register uint64_t reg0 asm("r29");
  register uint64_t reg1 asm("r14");
  register uint64_t reg2 asm("r12");
  register uint64_t reg3 asm("r15");

  register uint64_t reg8 asm("r3");
  register uint64_t reg9 asm("r4");
  register uint64_t reg10 asm("r5");
  register uint64_t reg11 asm("r6");
  register uint64_t reg12 asm("r7");
  register uint64_t reg13 asm("r8");
  register uint64_t reg14 asm("r9");
  register uint64_t reg15 asm("r10");

  uint64_t t = now_nsec();
  if (load) {
    for (i=0; i < outerloopcount; i++) {
      loop_count = (1<<(logbytes-9));
      SWEEP_FOOTPRINT_ppc(VECTORLOADppc, loop_count, m)
    }
  }
  else {
    for (i=0; i < outerloopcount; i++) {
      loop_count = (1<<(logbytes-9));
      SWEEP_FOOTPRINT_ppc(VECTORSTOREppc, loop_count, m)
    }
  }
  result.metric = (double)outerloopcount*(1<<(logbytes-4))/(now_nsec()-t);
  result.resulthash = outerloopcount;
  strcpy(result.metricname, load? "Billion_16_byte_loads_per_sec" : "Billion_16_byte_stores_per_sec");
#endif

#ifdef __aarch64__
  // ARM can do 16-bytes per load. To cover bytes you need...
  // bytes / 16 = bytes >> 4 = (1 << (logbytes - 4)) loads

  // Each iteration of loop_start_max_vector_loads_at_footprint in the inline assembly
  // will do 32 loads.
  // loop_count  = (1<< (logbytes-4))/32 = (1 << (logbytes-9))

  register uint64_t index0, index1, index2, index3, post_index;

  uint64_t t = now_nsec();
  if (load) {
    for (i=0; i < outerloopcount; i++) {
      loop_count = (1<<(logbytes-9));
      SWEEP_FOOTPRINT_arm(VECTORLOADarm, loop_count,
                          index0, index1, index2, index3,
                          post_index, m)
    }
  }
  else {
    for (i=0; i < outerloopcount; i++) {
      loop_count = (1<<(logbytes-9));
      SWEEP_FOOTPRINT_arm(VECTORSTOREarm, loop_count,
                          index0, index1, index2, index3,
                          post_index, m)
    }
  }
  result.metric = (double)outerloopcount*(1<<(logbytes-4))/(now_nsec()-t);
  result.resulthash = outerloopcount;
  strcpy(result.metricname, load? "Billion_16_byte_loads_per_sec" : "Billion_16_byte_stores_per_sec");

#endif
  return result;
}
