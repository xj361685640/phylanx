# Copyright (c) 2017 R. Tohid
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

FROM stellargroup/hpx:dev

RUN apt-get -y install wget &&                                                   \
    apt-get -y install python3 python3-dev python3-pip &&                        \
    apt-get -y install libblas-dev liblapack-dev &&                              \
    pip3 install setuptools &&                                                   \
    pip3 install pytest &&                                                       \
    cd / &&                                                                      \
    git clone --depth=1 https://github.com/pybind/pybind11.git pybind11-src &&   \
    cd pybind11-src &&                                                           \
    mkdir build && cd build &&                                                   \
    cmake  -DPYBIND11_TEST=OFF -DCMAKE_INSTALL_PREFIX=/pybind11 .. &&            \
    make &&                                                                      \
    make install &&                                                              \
    cd / &&                                                                      \
    git clone --depth=1 https://bitbucket.org/blaze-lib/blaze.git blaze &&       \
    cd blaze &&                                                                  \
    cmake -DBLAZE_SMP_THREADS=C++11 . &&                                         \
    make
