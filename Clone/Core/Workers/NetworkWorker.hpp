#include "iWorker.hpp"
#include <asio.hpp>

namespace Clone::Workers {

/// @brief  NetworkWriter implement Server part. Keep connection
///          save data
class NetworkWriter final : public IWriter {
  int execute() override { return 42; };

  bool isDone() override { return true; };
}

/// @brief Role of reader is just Client like. Send data to some endpoint
class NetworkReader final : public IReader {
  int execute() override { return 42; };

  bool isDone() override { return true; };
}