#include "WorkerFactory.hpp"

#include <utility>

#include "Configuration.hpp"
#include "NetworkWorkerFactory.hpp"
#include "IPCWorkerFactory.hpp"
#include "BytestreamWorkerFactory.hpp"
#include "Utils.hpp"

namespace Clone {

namespace fs = std::filesystem;

using Workers::IPCReader;
using Workers::IPCWriter;
using Workers::IReader;
using Workers::IWorker;
using Workers::IWriter;
using Parser::CopyingMode;
using Prop = Parser::CfgProperties;

WorkerFactory::WorkerFactory(std::shared_ptr<Parser::Configuration>& sp_cfg)
    : _config(std::move(sp_cfg))
{
}

IWorker* WorkerFactory::getWorker()
{
  auto factory = getFactory(_config->get_param<CopyingMode>(Prop::mode));
  auto cc = _config->get_param(Prop::role);
  if (  cc == "HOST")
    return factory->CreateWriter();
  else
    return factory->CreateReader();
}

AbstractWorkerFactory* Clone::WorkerFactory::getFactory(CopyingMode mode)
{
  switch (mode) {
    case CopyingMode::BitStream:
      return new BytestreamWorkerFactory(_config->get_param<std::string>(Prop::source),
                                         _config->get_param<std::string>(Prop::destination));

    case CopyingMode::SharedMemoryStream:
      return new AbstractIPCFactory(_config->get_param<std::string>(Prop::source));

    case CopyingMode::NetworkSharing:
      return new NetworkWorkerFactory(_config->get_param<std::string>(Prop::source),
                                      _config->get_param<std::string>(Prop::destination),
                                      _config->get_param<std::string>(Prop::port));

    default:
      break;
  }
  return nullptr;
}

}  // namespace Clone
