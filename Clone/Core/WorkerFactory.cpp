#include "WorkerFactory.hpp"

#include <assert.h>

#include <memory>
#include <typeinfo>

#include "Configuration.hpp"
#include "Utils.hpp"

namespace Clone {

namespace fs = std::filesystem;

using Workers::ByteStreamWorker;
using Workers::IPCReader;
using Workers::IPCWorker;
using Workers::IPCWriter;
using Workers::IReader;
using Workers::IWorker;
using Workers::IWriter;
using Prop = Parser::CfgProperties;

WorkerFactory::WorkerFactory(std::shared_ptr<Parser::Configuration> sp_cfg)
    : _config(sp_cfg)
{
}

IWorker* WorkerFactory::getWorker()
{
  auto factory = getFactory(_config->get_param( Prop::mode));
  if (_config->get_param(Prop::role) == "HOST")
    return factory->CreateWriter();
  else
    return factory->CreateReader();
}

AbstractWorkerFactory* Clone::WorkerFactory::getFactory(std::string mode)
{
  if (mode == "IPC")
    return getFactory(CopyingMode::SharedMemoryStream);
  else if (mode == "BITSTREAM")
    return getFactory(CopyingMode::BitStream);
  return nullptr;
}

AbstractWorkerFactory* Clone::WorkerFactory::getFactory(CopyingMode mode)
{
  switch (mode) {
    case Clone::WorkerFactory::CopyingMode::BitStream:
      return new BytestreamWorkerFactory(_config->get_param(Prop::source),
                                         _config->get_param(Prop::destination));

    case Clone::WorkerFactory::CopyingMode::SharedMemoryStream:
      return new AbstractIPCFactory( _config->get_param(Prop::source) );

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
  assert(("Need to implement", false));
  return reinterpret_cast<IReader*>(nullptr);
}

IWriter* BytestreamWorkerFactory::CreateWriter()
{
  assert(("Need to implement", false));
  return reinterpret_cast<IWriter*>(nullptr);
}

}  // namespace Clone
