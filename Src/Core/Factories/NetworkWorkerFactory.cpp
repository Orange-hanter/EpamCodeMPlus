//
// Created by daniil on 5/19/23.
//

#include "NetworkWorkerFactory.hpp"

#include "NetworkWorker.hpp"

namespace Clone {
Workers::IReader* NetworkWorkerFactory::CreateReader()
{
  return new Workers::NetworkReader(_adr, _port, _source);
}

Workers::IWriter* NetworkWorkerFactory::CreateWriter()
{
  return new Workers::NetworkWriter(_source, _port);
}
}  // namespace Clone