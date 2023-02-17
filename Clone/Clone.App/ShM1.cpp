#include <conio.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include <new>
#include <sstream>

#include "Frame.hpp"
#include "Utils.hpp"

// #define BUF_SIZE 256
// TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
// TCHAR szMsg[] = TEXT("Message from first process.");

int _tmain()
{
  auto pBuf = Clone::IPC::Frame();

  print_mem((void*)&*pBuf.data(), sizeof(*pBuf.data()));

  bool loop = true;
  while (loop) {
    char opcode;
    std::cout << ">>";
    std::cin >> opcode;
    switch (opcode) {
      case 'r':
        std::cout << "<<" << pBuf << '\n';
        break;
      case 'w':
        std::cout << ">>>";
        std::cin.clear();
        std::cin >> pBuf;
        std::cout << '\n';
        break;
      case 'q':
        loop = false;
        break;
      case 'i':
        print_mem((void*)&(*pBuf.data()), sizeof(*pBuf.data()));
        break;
      default:
        break;
    }
    std::cin.clear();
  }

  return 0;
}