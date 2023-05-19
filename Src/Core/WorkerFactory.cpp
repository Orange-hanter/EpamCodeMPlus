#include "WorkerFactory.hpp"

#include <assert.h>

#include <memory>
#include <typeinfo>
#include <utility>

#include "Configuration.hpp"
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

WorkerFactory::WorkerFactory(std::shared_ptr<Parser::Configuration> sp_cfg)
    : _config(std::move(sp_cfg))
{
}

IWorker* WorkerFactory::getWorker()
{
  auto factory = getFactory(_config->get_param<CopyingMode>(Prop::mode));
  if (_config->get_param<std::string>(Prop::role) == "HOST")
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
      return new NetworkWorkerFactory(_config->get_param<std::string>(Prop::source));

    default:
      break;
  }
  return nullptr;
}

//---------------------------------------------------------------------------------
IReader* AbstractIPCFactory::CreateReader() { return new IPCReader(_source); }

IWriter* AbstractIPCFactory::CreateWriter() { return new IPCWriter(_source); }

IReader* BytestreamWorkerFactory::CreateReader()
{
  assert(("Shouldn't be run", false));
  return nullptr;
}

IWriter* BytestreamWorkerFactory::CreateWriter()
{
  return new Workers::ByteStreamWriter(_source, _destination);
}

//------------------------------------------------------------------------------
IReader* NetworkWorkerFactory::CreateReader () { return new Workers::NetworkReader(); }

IWriter* NetworkWorkerFactory::CreateWriter () { return new Workers::NetworkWriter(
      asio::ip::tcp::acceptor(asio::any_io_executor())); }
  
}  // namespace Clone
