#include <WorkerFactory.hpp>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <strstream>
#include "Startup.hpp"

using Clone::WorkerFactory;
using namespace std::string_literals;

std::uint32_t start_dword = 0x0AA00AA0;
std::uint32_t end_dword = 0x05500550;

bool writeDelimitedTo(
    const google::protobuf::MessageLite& message,
    google::protobuf::io::ZeroCopyOutputStream* rawOutput) {
    // We create a new coded stream for each message.  Don't worry, this is fast.
    google::protobuf::io::CodedOutputStream output(rawOutput);

    // Write the size.
    const int size = message.ByteSizeLong();
    output.WriteVarint32(size);

    uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
    if (buffer != nullptr) {
        // Optimization:  The message fits in one buffer, so use the faster
        // direct-to-array serialization path.
        message.SerializeWithCachedSizesToArray(buffer);
    } else {
        // Slightly-slower path when the message is multiple buffers.
        message.SerializeWithCachedSizes(&output);
        if (output.HadError()) return false;
    }

    return true;
}

bool readDelimitedFrom(
    google::protobuf::io::ZeroCopyInputStream* rawInput,
    google::protobuf::MessageLite* message) {
    // We create a new coded stream for each message.  Don't worry, this is fast,
    // and it makes sure the 64MB total size limit is imposed per-message rather
    // than on the whole stream.  (See the CodedInputStream interface for more
    // info on this limit.)
    google::protobuf::io::CodedInputStream input(rawInput);

    // Read the size.
    uint32_t size;
    if (!input.ReadVarint32(&size)) return false;

    // Tell the stream not to read beyond that size.
    google::protobuf::io::CodedInputStream::Limit limit =
        input.PushLimit(size);

    // Parse the message.
    if (!message->MergeFromCodedStream(&input)) return false;
    if (!input.ConsumedEntireMessage()) return false;

    // Release the limit.
    input.PopLimit(limit);

    return true;
}

void do_write()
{
    Filetransfer::FileTransferRequest_t request;
    request.set_file_name("filename");
    request.set_file_size_kb(1234);
    request.set_md5_hash("asdasfcxvef");

    std::array<char, 256> buffer{0};
    //........................
    auto pos = buffer.begin();
    std::vector<unsigned char> b;
    std::fill_n(std::inserter(b, b.begin()), 4, );

    {
        std::ostrstream osf("message", std::ios_base::binary);
        osf.write(buffer.data(), buffer.size());
    }


}



Filetransfer::FileTransferRequest_t do_read()
{
    Filetransfer::FileTransferRequest_t response;

    return response;
}

int main(int argc, char** argv)
{




  auto conf = Clone::Parser::StartupArgumentsParser(argc, argv).getConfiguration();

  auto worker = Clone::WorkerFactory(conf).getWorker();

  worker->execute();

  while (!worker->isDone()) {
    /* wait until end */
  }

  return 0;
}