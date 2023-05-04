#pragma once
#include <argparse/argparse.hpp>
#include <memory>

#include "Configuration.hpp"

namespace Clone::Parser {
class StartupArgumentsParser {
 public:
  StartupArgumentsParser(int argc, char** argv)
  {
    _parser = std::make_unique<argparse::ArgumentParser>("Copy");
    _config = std::make_unique<Configuration>();

    _parser->add_description(
        "Copy tool app implement simple 2 threaded \"copy\" tool."
        "Tool should be implemented as a console application."
        "Tool should accept 2 parameters source filename and target filename."
        "Copying logic should be organized with help of 2 threads."
        "First thread should read the data from source file."
        "Second thread should write the data to the target file.");

    _parser->add_argument("-s", "--source")
        .required()
        .default_value<std::string>("-")
        .help("Absolute path to original file.");

    _parser->add_argument("-d", "--destination")
        .default_value<std::string>("-")
        .help("Absolute path to the new file.");

    _parser->add_argument("-f", "--frame")
        .help("Max size of one package.")
        .default_value<std::string>("32")
        .action([this](const std::string& value) {
          _config->set_param("frame", value);
        });

    _parser->add_argument("--ipc", "--mode")
        .help("Turn on interprocess communication mode.")
        .default_value<std::string>("BITESTREAM")
        .implicit_value(std::string("IPC"));

    _parser->add_argument("--client", "--role")
        .help(
            "Work only with --ipc mode. Take role of host, and fill shared "
            "memory by data. By default used host mode")
        .default_value<std::string>("HOST")
        .implicit_value(std::string("CLIENT"));

    try {
      _parser->parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
      std::cerr << err.what() << std::endl;
      std::cerr << _parser;
      std::exit(1);
    }
  }

  // deprecated
  std::string getParam(std::string_view parameter) const
  {
    return _parser->get(parameter);
  }

  std::shared_ptr<Configuration> getConfiguration()
  {
    auto set = [this](const std::string & param){
      auto key = std::string("--") + param;
      auto value = _parser->get(key);
      _config->set_param(param, value);
    };
    set(CfgProperties::source);
    set(CfgProperties::destination);
    set(CfgProperties::frame);
    set(CfgProperties::role);
    set(CfgProperties::mode);

    return _config;
  }

  bool isFlag(std::string_view flag) const { return _parser->is_used(flag); }

 private:
  std::unique_ptr<argparse::ArgumentParser> _parser;
  std::shared_ptr<Configuration> _config;
};
}  // namespace Clone::Parser