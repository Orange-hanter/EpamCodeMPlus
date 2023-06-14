//
// Created by daniil on 6/7/23.
//
#include "IPCWorkerFactory.hpp"

#include "IPCWorker.hpp"

namespace Clone
{

Workers::IReader* AbstractIPCFactory::CreateReader() { return new Workers::IPCReader(_source); }

Workers::IWriter* AbstractIPCFactory::CreateWriter() { return new Workers::IPCWriter(_source); }

}