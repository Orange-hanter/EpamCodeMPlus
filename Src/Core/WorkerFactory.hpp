#pragma once

#include <memory>
#include <string>
#include <utility>

#include "AbstractWorkerFactory.hpp"
#include "Workers.h"

namespace Clone
{

namespace Parser
{
class Configuration;
enum class CopyingMode;
}  // namespace Parser

class AbstractIPCFactory;
class BytestreamWorkerFactory;
class AbstractWorkerFactory;

using spIWorker = std::shared_ptr<Workers::IWorker>;

/**
 *    @brief Class that produce work
 */
class WorkerFactory
{
public:
    explicit WorkerFactory(std::shared_ptr<Parser::Configuration>& sp_cfg);
    ~WorkerFactory() = default;

    Clone::Workers::IWorker* getWorker();

private:
    // AbstractWorkerFactory* getFactory(std::string_view mode);
    AbstractWorkerFactory* getFactory(Parser::CopyingMode mode);

    std::shared_ptr<Parser::Configuration> _config;
};

}  // namespace Clone