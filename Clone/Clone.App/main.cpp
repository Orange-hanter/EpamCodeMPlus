#include <fstream>
#include <ios>
#include <iostream>
#include <string>

#include "Startup.hpp"
#include "WorkerFactory.hpp"

using Clone::WorkerFactory;

int main(int argc, char** argv)
{
  auto conf = Clone::StartupConfiguration(argc, argv);

  auto mode = conf.isFlag("--ipc")
                  ? WorkerFactory::CopyingMode::SharedMemoryStream
                  : WorkerFactory::CopyingMode::BitStream;

  auto worker = Clone::WorkerFactory(conf.getParam("--source"),
                                     conf.getParam("--destination"))
                    .getWorker(mode, conf.isFlag("--role_host"));

  worker->execute();

  while (!worker->isDone()) {
    /* wait until end */
  }

  return 0;
}