#!/bin/bash -eu
#
# Copyright 2018 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# --alu_latency --max_alu_ipc --max_scalar_load_ipc --max_vector_load_ipc --max_vector_loads_at_footprint 18 --max_scalar_store_ipc --max_vector_store_ipc --max_vector_stores_at_footprint 18 --taken_branch_latency --branch_history 128
ssh root@$1 "mkdir -p /data/$(whoami)/mpmtest/new" 
ssh root@$1 "mkdir -p /data/$(whoami)/mpmtest/mpm"

scp test-on-target.sh root@$1:/data/$(whoami)/mpmtest
scp $(pwd | sed "s/google3/google3\/blaze-bin/g")/cputest root@$1:/data/$(whoami)/mpmtest/new/

ssh root@$1 "cd /data/$(whoami)/mpmtest; ./test-on-target.sh \"$2\""
