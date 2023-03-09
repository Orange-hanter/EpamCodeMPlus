#include <chrono>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <vector>

#include "WorkerFactory.hpp"

using namespace std::chrono;
using std::filesystem::path;

static const uint32_t KByte_s = 1024;
static const uint32_t MByte_s = KByte_s * 1024;
static const uint32_t testingMemoryBlockCount = 100;
static const uint32_t testingMemoryBlockSize = MByte_s;

void generateFile(const path& sourceFile);

int main(int argc, char** arv)
{
  const path sourceFile = "src.data";
  const path destinationFile = "dst.data";

  generateFile(sourceFile);

  std::vector<long long> time;

  for (int i = 0; i < 10; i++) {
    auto start = steady_clock::now();

    Clone::WorkerFactory(absolute(sourceFile).string(),
                         absolute(destinationFile).string())
        .getWorker(Clone::WorkerFactory::CopyingMode::BitStream)
        ->execute();

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

void generateFile(const path& sourceFile)
{
  static std::array<unsigned char, testingMemoryBlockCount> mem;
  mem.fill(0x55);
  std::ofstream file(sourceFile);
  for (auto const ch : mem) file << ch;
  file.close();
}