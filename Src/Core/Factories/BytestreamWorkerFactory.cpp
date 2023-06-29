//
// Created by daniil on 6/7/23.
//

#include "BytestreamWorkerFactory.hpp"

#include <cassert>

#include "ByteStreamWorker.hpp"

namespace Clone
{

  Workers::IReader* BytestreamWorkerFactory::CreateReader()
  {
    assert(((void)"Shouldn't be run", false));
    return nullptr;
  }

  Workers::IWriter* BytestreamWorkerFactory::CreateWriter()
  {
    return new Workers::ByteStreamWriter(_source, _destination);
  }

}
