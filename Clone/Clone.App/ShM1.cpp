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
//   HANDLE hMapFile;
//   pFrame_t pBuf;

//   hMapFile =
//       CreateFileMapping(INVALID_HANDLE_VALUE,  // use paging file
//                         NULL,                  // default security
//                         PAGE_READWRITE,        // read/write access
//                         0,         // maximum object size (high-order DWORD)
//                         BUF_SIZE,  // maximum object size (low-order DWORD)
//                         szName);   // name of mapping object

//   if (hMapFile == NULL) {
//     _tprintf(TEXT("Could not create file mapping object (%d).\n"),
//              GetLastError());
//     return 1;
//   }
//   pBuf = (pFrame_t)MapViewOfFile(hMapFile,             // handle to map object
//                                  FILE_MAP_ALL_ACCESS,  // read/write permission
//                                  0, 0, BUF_SIZE);

//   if (pBuf == NULL) {
//     _tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());

//     CloseHandle(hMapFile);

//     return 1;
//   }

  //new (pBuf) Clone::Frame<char>();
  auto pBuf = Clone::InplaceFrame();
  std::stringstream ss;
  ss << "Hello!!";

  // CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
  print_mem((void*)&pBuf, sizeof(pBuf));

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