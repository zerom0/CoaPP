# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# docker build --rm -t coapp .

# docker run --rm coapp -v `pwd`:/src

FROM ubuntu:12.04

RUN apt-get install -qq python-software-properties
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-get -qq update

# Install CMake
#
RUN apt-get install -qq cmake make
RUN cmake --version

# Install g++ v4.8
#
RUN apt-get install -qq g++-4.8
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20
RUN update-alternatives --config gcc
RUN update-alternatives --config g++
RUN g++ --version

VOLUME ["/src"]

CMD mkdir -p /src/testbuild && cd /src/testbuild && cmake .. && make && ctest --output-on-error
