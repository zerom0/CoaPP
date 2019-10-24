#include <Message.h>
#include <cassert>
#include <iostream>
#include <fstream>

// afl-clang++ -g -I ../src -I ../include/ --std=c++14 -fsanitize=address ./fuzzer.cpp ../src/Message.cpp ../src/Path.cpp
// afl-fuzz -i testcases -o findings ./a.out @@

int main(int argc, char **argv) {
  assert (argc > 1);
  std::ifstream file(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  assert (size >= 0);
  file.seekg(0, std::ios::beg);
  CoAP::Message::Buffer buffer(size);
  file.read((char *) buffer.data(), size);
  file.close();

  try {
    auto m = CoAP::Message::fromBuffer(buffer);
    auto b = m.asBuffer();

  } catch (std::exception &e) {
  }
  return 0;
}