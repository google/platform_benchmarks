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

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cputest.h"
#include "third_party/platform_benchmarks/result.h"

static void helpinfo() {
  int i;
  for (i = 0; i < NUMTESTS; i++) {
    printf("%40s\t%s\n", alltests[i].name, alltests[i].helpinfo);
  }
}

static unsigned int convert_or_crash(const char *input) {
  char *endptr;
  unsigned int result = strtoul(input, &endptr, 0);

  if (endptr[0] != '\0') {
    fprintf(stderr, "%s is not a number\n", input);
    exit(1);
  }

  if (errno == ERANGE) {
    fprintf(stderr, "%s is out of range\n", input);
    exit(1);
  }
  return result;
}

int main(int argc, char* argv[]) {
  int i;
  if (argc < 2) {
    helpinfo();
  }

  for (i = 1; i < argc;) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      helpinfo();
      break;
    }
    int j;
    int test_not_found = 1;
    for (j = 0; j < NUMTESTS; j++) {
      if (strchr(alltests[j].name, ' ') ?
          (strncmp(argv[i],
                   alltests[j].name,
                   strchr(alltests[j].name, ' ') - alltests[j].name) == 0):
          (strcmp(argv[i], alltests[j].name) == 0)) {
        test_not_found = 0;
        struct Result r;
        switch (alltests[j].args) {
          case 0:
            r = alltests[j].function();
            break;
          case 1:
            r = alltests[j].function(convert_or_crash(argv[i + 1]));
            break;
          case 2:
            r = alltests[j].function(convert_or_crash(argv[i + 1]),
                                     convert_or_crash(argv[i + 2]));
            break;
          case 3:
            r = alltests[j].function(convert_or_crash(argv[i + 1]),
                                     convert_or_crash(argv[i + 2]),
                                     convert_or_crash(argv[i + 3]));
            break;
          case 4:
            r = alltests[j].function(convert_or_crash(argv[i + 1]),
                                     convert_or_crash(argv[i + 2]),
                                     convert_or_crash(argv[i + 3]),
                                     convert_or_crash(argv[i + 4]));
            break;
          case 5:
            r = alltests[j].function(convert_or_crash(argv[i + 1]),
                                     convert_or_crash(argv[i + 2]),
                                     convert_or_crash(argv[i + 3]),
                                     convert_or_crash(argv[i + 4]),
                                     convert_or_crash(argv[i + 5]));
            break;
          default:
            perror("Unsupported number of arguments\n");
        };
        printf("%s\tresulthash=%" PRIx64 "\t%s=%.6f\n", r.function,
               r.resulthash, r.metricname, r.metric);
        i += alltests[j].args;
        break;
      }
    }
    if (test_not_found) {
      fprintf(stderr, "%s test not found\n", argv[i]);
      exit(1);
    }
    ++i;
  }
  return 0;
}
