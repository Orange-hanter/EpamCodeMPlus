#include <catch2/catch_all.hpp>

//#include "Direcror.hpp"

TEST_CASE("Factorial of 0 is 1 (fail)", "[single-file]")
{
  // Clone::Director director(conf.getParam<std::string>("--source"),
  //                          conf.getParam<std::string>("--destination"));
  REQUIRE(0 == 1);
}

TEST_CASE("Factorials of 1 and higher are computed (pass)", "[single-file]")
{
  REQUIRE(1 == 1);
}
