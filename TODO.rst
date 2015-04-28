TODO
====

Immediate:
----------

* fix bug with init from string needing > 32 bits (start by testing 1. add two numbers > 2**32... 2. multiply two numbers > 2**32)

Later
-----

* add tests for hard limits for .bits()
* - try to write independent of chunk size / initial chunks
* add negative tests for += and *=
* fill in tests for .all_bits()
* add unittest-cpp as remote
