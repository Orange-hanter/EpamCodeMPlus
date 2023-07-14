#ifdef WIN32
#include <windows.h>
#else
using HANDLE = void*;
#endif

#include <array>
#include <cstring>
#include <iostream>
#include <istream>
#include <memory>
#include <mutex>
#include <sstream>

namespace Clone
{
using byte = char;
};

namespace Clone::IPC
{

class Frame
{
    static constexpr std::size_t m_chunk_size = 64;
public:
    enum class STATE
    {
        START,
        INITIALISED,
        READY_TO_WORK,
        READY_TO_READ,
        READY_TO_WRITE,
        END_SOURCE,
        DONE,
        TERMINATE
    };

    struct DataBulk
    {
        STATE _state{STATE::START};
        std::size_t _sizeOfData{0};
        byte _data_bulk[m_chunk_size]{0};
    };

    using T    = DataBulk;
    using TRef = T*;
#ifdef WIN32
    Frame()
    {
        hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,  // use paging file
                                     nullptr,               // default security
                                     PAGE_READWRITE,        // read/write access
                                     0,                     // maximum object size (high-order DWORD)
                                     defaultFrameSize,      // maximum object size (low-order DWORD)
                                     TEXT(szName));         // name of mapping object

        if (hMapFile == nullptr)
        {
            exit(2);  // TODO do some mechanism with errors
        }

        _buf = (TRef)MapViewOfFile(hMapFile,             // handle to map object
                                   FILE_MAP_ALL_ACCESS,  // read/write permission
                                   0, 0, defaultFrameSize);

        if (_buf == nullptr)
        {
            auto err = GetLastError();  // the same, need to save error and check after
            if (!err)
            {
                CloseHandle(hMapFile);
            }
            exit(3);
        }
        _old_handler = set_terminate([]() {
            HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,  // read/write access
                                              FALSE,                // do not inherit the name
                                              TEXT(szName));        // name of mapping object
            if (hMapFile != NULL)
            {
                auto pMem    = (TRef)MapViewOfFile(hMapFile,             // handle to file mapping object
                                                   FILE_MAP_ALL_ACCESS,  // read/write access
                                                   0, 0, defaultFrameSize);
                pMem->_state = STATE::TERMINATE;
                UnmapViewOfFile(pMem);
                CloseHandle(hMapFile);
            }
            std::cout << "Memory utilized. Sayanara!";
        });

        if (isState(STATE::START))
        {
            new (_buf) T();
            setState(STATE::INITIALISED);
        }
        else if (isState(STATE::INITIALISED))
        {
            setState(STATE::READY_TO_WORK);
        }
        else
        {
            exit(4);
        }
    }

    ~Frame()
    {
        set_terminate(_old_handler);
        UnmapViewOfFile(_buf);
        CloseHandle(hMapFile);
    }
#else
    Frame()  = default;
    ~Frame() = default;
#endif

    TRef data() const { return _buf; }

    bool isState(STATE state)
    {
        std::lock_guard<std::mutex> lock(_m);
        return _buf->_state == state;
    }

    void setState(STATE state)
    {
        std::lock_guard<std::mutex> lock(_m);
        // std::cout << "\nState " << static_cast<int>(_buf->_state) << " to "
        //           << static_cast<int>(state) << '\n';
        _buf->_state = state;
    }

    void logDataBulk() const
    {
#ifdef DEBUG
        std::cout << "Size of block " << _buf->_sizeOfData << "\nState " << static_cast<int>(_buf->_state) << "\nBulk data\n"
                  << _buf->_data_bulk;
#endif
    }

    friend std::basic_istream<byte>& operator>>(std::basic_istream<byte>& ifs, Frame& frame)
    {
        // TODO look at moment with size of read memory, is it might be more than m_chunk_size
        ifs.read(frame._buf->_data_bulk, m_chunk_size - frame._buf->_sizeOfData);
        frame._buf->_sizeOfData = static_cast<size_t>(ifs.gcount());
        return ifs;
    }

    friend std::basic_istream<byte>& operator>>(std::basic_istream<byte>& ifs, Frame* frame)
    {
        ifs.read(frame->_buf->_data_bulk, m_chunk_size - frame->_buf->_sizeOfData);
        frame->_buf->_sizeOfData = static_cast<size_t>(ifs.gcount());
        return ifs;
    }

    friend std::basic_ostream<byte>& operator<<(std::basic_ostream<byte>& ofs, const Frame* frame)
    {
        ofs.write(frame->_buf->_data_bulk, frame->_buf->_sizeOfData);
        return ofs;
    }

    friend std::basic_ostream<byte>& operator<<(std::basic_ostream<byte>& ofs, const Frame& frame)
    {
        ofs.write(frame._buf->_data_bulk, frame._buf->_sizeOfData);
        return ofs;
    }

    Frame(const Frame*)             = delete;
    Frame(Frame*)                   = delete;
    Frame& operator=(const Frame*)  = delete;
    Frame(const Frame&&)            = delete;
    Frame& operator=(const Frame&&) = delete;

private:
    static inline const char szName[]    = {"Global\\MyFileMappingObject"};
    static const size_t defaultFrameSize = sizeof(DataBulk);
    [[maybe_unused]] HANDLE hMapFile{};
    std::terminate_handler _old_handler{};
    std::mutex _m;
    TRef _buf{};
};
}  // namespace Clone::IPC