#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>

#include "../Clone.App/Director.hpp"

using namespace std::chrono;
using namespace std::filesystem;

static const uint32_t KByte_s = 1024;
static const uint32_t MByte_s = KByte_s * 1024;
static const uint32_t testingMemorySize = 100;
static char mem[MByte_s];

void generateFile(path sourceFile)
{
  std::ofstream file(sourceFile);
  for (size_t i = 0; i < testingMemorySize; i++) file << mem;
  // file << '\0';
  file.close();
}

int main(int argc, char** arv)
{
  std::fill_n(mem, MByte_s, 0x55);
  path sourceFile = "src.data";
  path destinationFile = "dst.data";

  generateFile(sourceFile);

  auto start = steady_clock::now();
  {
    Clone::Director director(absolute(sourceFile).string(),
                             absolute(sourceFile).string());
    director.work();
  }
  auto end = steady_clock::now();

  remove(sourceFile);
  remove(destinationFile);

  std::cout << "Elapsed time in microseconds: "
            << duration_cast<microseconds>(end - start).count() << " ms"
            << std::endl;
  return 0;
}