#!/bin/sh

set -xe

(test -e build || mkdir build) && cd build && cmake -DCMAKE_BUILD_TYPE=RelWithDebugInfo .. && make -j && ctest . -VV
