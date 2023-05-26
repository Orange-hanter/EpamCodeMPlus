//
// Created by daniil on 5/19/23.
//

#include "NetworkWorkerFactory.hpp"

#include "NetworkWorker.hpp"

namespace Clone {
Workers::IReader* NetworkWorkerFactory::CreateReader () { return new Workers::NetworkReader("127.0.0.1", "9080", "~/file"); }

Workers::IWriter* NetworkWorkerFactory::CreateWriter () { return new Workers::NetworkWriter("~/.file_repo", "9080"); }
}  // namespace Clone