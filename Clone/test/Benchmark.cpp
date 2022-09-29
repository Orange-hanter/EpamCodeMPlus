#include <chrono>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <vector>

#include "../Clone.App/Director.hpp"

using namespace std::chrono;
using std::filesystem::path;

static const uint32_t KByte_s = 1024;
static const uint32_t MByte_s = KByte_s * 1024;
static const uint32_t testingMemoryBlockCount = 100;
static const uint32_t testingMemoryBlockSize = MByte_s;

void generateFile(path sourceFile)
{
  static char mem[testingMemoryBlockSize];
  std::fill_n(mem, testingMemoryBlockSize, 0x55);
  std::ofstream file(sourceFile);
  for (size_t i = 0; i < testingMemoryBlockCount; i++) file << mem;
  file.close();
}

int main(int argc, char** arv)
{
  path sourceFile = "src.data";
  path destinationFile = "dst.data";

  generateFile(sourceFile);

  std::vector<int> time;

  for (int i = 0; i < 10; i++) {
    auto start = steady_clock::now();
    {
      Clone::Director director(absolute(sourceFile).string(),
                               absolute(destinationFile).string());
      director.work();
    }
    auto end = steady_clock::now();
    time.push_back(duration_cast<milliseconds>(end - start).count());
    std::cout << "Attempt " << i << ' ' << time[i] << " ms\n";
    remove(destinationFile);
  }
  remove(sourceFile);
  std::cout << "\nAverage time of copying is: "
            << std::accumulate(time.begin(), time.end(), 0) / time.size()
            << "ms";
  return 0;
}