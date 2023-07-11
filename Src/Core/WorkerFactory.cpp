#include "WorkerFactory.hpp"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup.hpp>
#include <utility>

#include "BytestreamWorkerFactory.hpp"
#include "Configuration.hpp"
#include "IPCWorkerFactory.hpp"
#include "NetworkWorkerFactory.hpp"
#include "Utils.hpp"

namespace Clone
{

namespace fs      = std::filesystem;
namespace logging = boost::log;

using Workers::IPCReader;
using Workers::IPCWriter;
using Workers::IReader;
using Workers::IWorker;
using Workers::IWriter;

using Parser::CopyingMode;
using Property = Parser::CfgProperties;

WorkerFactory::WorkerFactory(std::shared_ptr<Parser::Configuration>& sp_cfg) : _config(std::move(sp_cfg)) {}

IWorker* WorkerFactory::getWorker()
{
    auto factory = getFactory(_config->get_param<CopyingMode>(Property::mode));
    auto cc      = _config->get_param(Property::role);

    // logging::add_file_log(cc + "sample.log");
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::trace);

    return cc == "HOST" ? factory->CreateWriter() : factory->CreateReader();
}

AbstractWorkerFactory* Clone::WorkerFactory::getFactory(CopyingMode mode)
{
    switch (mode)
    {
        case CopyingMode::BitStream:
            return new BytestreamWorkerFactory(_config->get_param<std::string>(Property::source),
                                               _config->get_param<std::string>(Property::destination));

        case CopyingMode::SharedMemoryStream:
            return new AbstractIPCFactory(_config->get_param<std::string>(Property::source));

        case CopyingMode::NetworkSharing:
            return new NetworkWorkerFactory(_config->get_param<std::string>(Property::source),
                                            _config->get_param<std::string>(Property::destination),
                                            _config->get_param<std::string>(Property::port));

        default:
            break;
    }
    return nullptr;
}

}  // namespace Clone
