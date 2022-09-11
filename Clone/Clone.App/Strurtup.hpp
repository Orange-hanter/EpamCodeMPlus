#pragma once
#include <argparse/argparse.hpp>
#include <memory>

namespace Clone {
class StrurtupConfiguration {
 public:
  StrurtupConfiguration(int argc, char** argv)
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
        .help("Absolute path to original file.");

    _parser->add_argument("-o", "--destination")
        .required()
        .help("Absolute path to the new file.");

    _parser->add_argument("-f", "--frame")
        .help("Max size of one package.")
        .default_value<uint16_t>(32);

    try {
      _parser->parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
      std::cerr << err.what() << std::endl;
      std::cerr << _parser;
      std::exit(1);
    }
  }

  std::string getParam(std::string parameter)
  {
    if (auto arg = _parser->present(parameter)) {
      return arg.value();
    }
    throw std::exception("Such argument didn't founded");
    return {};
  }

 private:
  std::unique_ptr<argparse::ArgumentParser> _parser;
};
}  // namespace Clone