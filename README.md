[![Build Status](https://travis-ci.org/zerom0/CoaPP.svg?branch=master)](https://travis-ci.org/zerom0/CoaPP)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/8067/badge.svg)](https://scan.coverity.com/projects/zerom0-coapp)
[![MPLv2 License](https://img.shields.io/badge/license-MPLv2-blue.svg?style=flat-square)](https://www.mozilla.org/MPL/2.0/)

# CoaPP

C++11 implementation of the CoAP

# Running the testsuite

Install googletest before building with cmake

    git clone https://github.com/google/googletest.git gtest

Execute the tests with

    ctest --output-on-failure
