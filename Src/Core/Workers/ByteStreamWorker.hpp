#include "iWorker.hpp"

namespace Clone::Workers
{

class ByteStreamWriter final : public IWriter
{
public:
    ByteStreamWriter() = delete;

    explicit ByteStreamWriter(const std::string& src, const std::string& dst)
        : _dst(std::make_unique<std::ofstream>(dst, ios::binary)), _src(std::make_unique<std::ifstream>(src, ios::binary))
    {
    }

    int execute() override
    {
        startProcedureOfWriting();
        readDataToBuf();
        return 0;
    }

    bool isDone() override { return _done; }

private:
    inline void startProcedureOfWriting()
    {
        writer = std::jthread([this]() {
            while (!_src->eof() || !_packages.empty())
            {
                Frame pkg;
                _packages.pop_front_waiting(pkg);
                (*_dst).write(pkg.data(), pkg.frameSize());
            }

            _src->close();
            _dst->close();
            _done = true;
        });
    }

    inline void readDataToBuf()
    {
        while (!_src->eof())
        {
            Frame frame;
            (*_src) >> frame;
            _packages.push_back(std::move(frame));
            cv.notify_one();
        }
        cv.notify_one();
    }

private:
    bool _done = false;
    std::mutex m;
    std::condition_variable cv;
    std::jthread writer;
    std::unique_ptr<std::ofstream> _dst;
    std::unique_ptr<std::ifstream> _src;
    ThreadSafeDeque<Frame<>> _packages;
};
}  // namespace Clone::Workers