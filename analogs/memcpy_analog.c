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

#include "third_party/platform_benchmarks/analogs/exegesis_memcpy.h"
#include "third_party/platform_benchmarks/analogs/local_memcpy.h"
#include "third_party/platform_benchmarks/analogs/memcpy_analog.h"
#include "third_party/platform_benchmarks/result.h"
#include "third_party/platform_benchmarks/util.h"
#include "third_party/platform_benchmarks/x86_primitives.h"

#define MBPS(bytes,ns) ((double)((bytes>>20)*1000000000)/ns)

const int cacheline_size = 64;

enum VariantType {
  test_repmovs,
  test_memcpy,
};

struct Variant {
  const char* flag;
  const char* name;
  const char* helpinfo;
  uint64_t (*function)();
};


uint64_t rep_movs_time(uint64_t outer_loop_count,
                       uint64_t copy_bytes,
                       uint64_t src_pointer,
                       uint64_t dst_pointer) {
  uint64_t t = now_nsec();

  REP_MOVS_LOOP(outer_loop_count, copy_bytes, src_pointer, dst_pointer,
                "cld\n\t", "rep movsb\n\t", "\n")
  return now_nsec() - t;
}

uint64_t memcpy_time(uint64_t outer_loop_count,
                     uint64_t copy_bytes,
                     uint64_t src_pointer,
                     uint64_t dst_pointer) {
  void* srcptr = (void *)src_pointer;
  void* dstptr = (void *)dst_pointer;
  size_t n = (size_t)copy_bytes;

  uint64_t t = now_nsec();

  uint64_t i;
  for (i = 0; i < outer_loop_count; i++) {
    memcpy(dstptr, srcptr, n);
  }

  return now_nsec() - t;
}

uint64_t google3_strings_memcpy_time(uint64_t outer_loop_count,
                                     uint64_t copy_bytes,
                                     uint64_t src_pointer,
                                     uint64_t dst_pointer) {
  void* srcptr = (void *)src_pointer;
  void* dstptr = (void *)dst_pointer;
  size_t n = (size_t)copy_bytes;

  uint64_t t = now_nsec();

  uint64_t i;
  for (i = 0; i < outer_loop_count; i++) {
    google3_strings_memcpy(dstptr, srcptr, n);
  }

  return now_nsec() - t;
}

uint64_t exegesis_memcpy_time(uint64_t outer_loop_count,
                              uint64_t copy_bytes,
                              uint64_t src_pointer,
                              uint64_t dst_pointer) {
  void* srcptr = (void *)src_pointer;
  void* dstptr = (void *)dst_pointer;
  size_t n = (size_t)copy_bytes;

  uint64_t t = now_nsec();

  uint64_t i;
  for (i = 0; i < outer_loop_count; i++) {
    exegesis_memcpy(dstptr, srcptr, n);
  }

  return now_nsec() - t;
}

#define NUMTESTS 4
const struct Variant alltests[NUMTESTS] = {
  {"--repmovs", "repmovs", "rep movs performance", rep_movs_time},
  {"--memcpy", "memcpy", "memcpy performance", memcpy_time},
  {"--google3_strings_memcpy",
   "google3_strings_memcpy",
   "cs/strings/fastmem.h \"memcpy_inlined\" performance",
   google3_strings_memcpy_time},
  {"--google3_memcpy", "exegesis_memcpy", "exegesis memcpy performance",
   exegesis_memcpy_time},
};

struct Result search_memcpy_analog(const struct Variant* t) {
  struct Result result;

  void *m;

  if (posix_memalign(&m, cacheline_size,
                     LOOP4M * 2 + 2 * cacheline_size) != 0) {
    perror("Unable to align to cacheline size\n");
  }

  uint64_t total_time = 0;
  uint64_t last_total_time = 0;
  uint64_t total_copy_bytes = 0;
  int num_profile_entries = (sizeof(muppet_profile_cl193114178) /
                             sizeof(muppet_profile_cl193114178[0]));
  const struct MemcpyProfile* profile = muppet_profile_cl193114178;

  uint64_t src_pointer;
  uint64_t dst_pointer;
  uint64_t copy_bytes;
  uint64_t outer_loop_count;

  int p;
  for (p = 0; p < num_profile_entries; p++) {
    copy_bytes = profile[p].copy_bytes;
    total_copy_bytes += copy_bytes;
    outer_loop_count = profile[p].copy_count;
    src_pointer = ((uint64_t)m + profile[p].src_mod_64);

    // dst_pointer starts on a cacheline separate from src copy range.
    dst_pointer = ((uint64_t)m +
                   ((profile[p].src_mod_64 + copy_bytes) /
                    cacheline_size
                    + 1) *
                   cacheline_size +
                   profile[p].dst_mod_64);
    uint64_t (*variant)() = t->function;
    uint64_t elapsed_nsec = variant(outer_loop_count, copy_bytes, src_pointer, dst_pointer);
    total_time += elapsed_nsec;

    if ((total_time>>33) != (last_total_time>>33)) {
      printf("%s(%s): progress=%.0f%% seconds=%.0f MBPS=%.4f\n",
             __FUNCTION__, t->name, (double)(p*100)/num_profile_entries,
             (double)total_time/1000000000, MBPS(total_copy_bytes,total_time));
    }
    last_total_time = total_time;
  }

  result.metric = MBPS(total_copy_bytes, total_time);
  strcpy(result.metricname, "MBPS");
  sprintf(result.function, "%s(%s)",__FUNCTION__,t->name);
  return result;
}

void helpinfo() {
  int i;
  for (i = 0; i < NUMTESTS; i++) {
    printf("%40s\t%s\n", alltests[i].flag, alltests[i].helpinfo);
  }
}


int main(int argc, char* argv[]) {
  int i;
  for (i=1; i < argc;) {
    if (strcmp(argv[i], "--help")==0) {
      helpinfo();
      break;
    }
    int j;
    for (j = 0; j < NUMTESTS; j++) {
      if (strcmp(argv[i], alltests[j].flag)==0) {
        struct Result r = search_memcpy_analog(&alltests[j]);
        printf("%s %s=%.4f\n", r.function, r.metricname, r.metric);
      }
    }
    i++;
  }
}
