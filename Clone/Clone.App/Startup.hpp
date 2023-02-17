#pragma once
#include <argparse/argparse.hpp>
#include <memory>

namespace Clone {
class StartupConfiguration {
 public:
  StartupConfiguration(int argc, char** argv)
  {
    _parser = std::make_unique<argparse::ArgumentParser>("Copy");

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

    _parser->add_argument("-o", "--destination")
        .default_value<std::string>("-")
        .help("Absolute path to the new file.");

    _parser->add_argument("-f", "--frame")
        .help("Max size of one package.")
        .default_value<uint16_t>(32);

    _parser->add_argument("--ipc")
        .help("Turn on interprocess communication mode.")
        .default_value(false)
        .implicit_value(true);

    _parser->add_argument("--role_host")
        .help(
            "Work only with --ipc mode. Take role of host, and fill shared "
            "memory by data")
        .default_value(true)
        .implicit_value(true);

    _parser->add_argument("--role_client")
        .help(
            "Work only with --ipc mode. Take role of client, read data from "
            "shared memory")
        .default_value(false)
        .implicit_value(true);

    try {
      _parser->parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
      std::cerr << err.what() << std::endl;
      std::cerr << _parser;
      std::exit(1);
    }
  }

  std::string getParam(std::string_view parameter) const
  {
      return _parser->get(parameter);
  }

  bool isFlag(std::string_view flag) const { return _parser->is_used(flag); }

 private:
  std::unique_ptr<argparse::ArgumentParser> _parser;
};
}  // namespace Clone