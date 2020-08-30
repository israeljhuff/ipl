Israel's Programming Language (IPL)
Author: Israel Huff
https://github.com/israeljhuff/ipl

The parser is built using IPG.
A quick test program can be built using example_main.cpp from the IPG repo.

Assuming IPG is compiled and located at "../ipg/ipg.exe", compile and test like so:
  ../ipg/ipg.exe ipl.grammar > IPLparser.h
  g++ --std=c++17 -I../ipg test_main.cpp -o test_ipl.exe
  ./ipl.exe examples/sample.ipl
