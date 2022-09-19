#include <fstream>
#include <ios>
#include <iostream>
#include <string>

#include "Director.hpp"
#include "Strurtup.hpp"

int main(int argc, char** argv)
{
  auto conf = Clone::StrurtupConfiguration(argc, argv);

  Clone::Director director(conf.getParam("--source"),
                           conf.getParam("--destination"));
  director.work();
  return 0;
}