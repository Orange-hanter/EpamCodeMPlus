#include <fstream>
#include <ios>
#include <iostream>
#include <string>

#include "Startup.hpp"
#include "WorkerFactory.hpp"

using Clone::WorkerFactory;

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