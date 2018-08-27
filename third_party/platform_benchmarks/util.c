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

#include "util.h"

void byte2string(char* str, const unsigned logbytes) {
  unsigned num = (logbytes >= 30 ?
                  (1u << (logbytes-30)) :
                  (logbytes >= 20 ?
                   (1u << (logbytes - 20)) :
                   (logbytes >= 10) ?
                   (1u << (logbytes - 10)) : (1 << logbytes)));
  char sizestr[10];
  snprintf(sizestr, sizeof(sizestr), "%s", (logbytes >= 30 ?
                               "GB" :
                               (logbytes >= 20 ?
                                "MB" :
                                (logbytes >= 10) ? "KB" : "bytes")));
  snprintf(str, MAX_BYTE_STRING_LENGTH, "%u %s", num, sizestr);
}

void* randmemset(void *s, size_t n, unsigned randseed) {
  size_t i;
  char* p = (char *)s;
  for (i=0; i < n; i++) {
    p[i] = (rand_r(&randseed)>>16) & 0xff;
  }
  return s;
}
