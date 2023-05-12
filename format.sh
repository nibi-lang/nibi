#!/bin/bash
find ./nibi -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
