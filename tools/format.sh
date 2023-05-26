#!/bin/bash
find ./nibi -not -path "./nibi/lib/libnibi/parallel_hashmap/*" -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
