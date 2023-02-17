#include <conio.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include <iostream>
#include <new>

#pragma comment(lib, "user32.lib")

#include "Frame.hpp"
#include "Utils.hpp"

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[] = TEXT("LOOOOOL.");

int _tmain()
{
 
  auto pBuf = Clone::Frame();

  bool loop = true;
  while (loop) {
    char opcode;
    std::cout << ">>";
    std::cin >> opcode;
    switch (opcode) {
      case 'r':
        std::cout << "<<" << pBuf.data() << '\n';
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
          print_mem((void*)&pBuf, sizeof(pBuf));
          break;
      default:
        break;
    }
    std::cin.clear();
  }

  return 0;
}