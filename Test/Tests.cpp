#include <catch2/catch_all.hpp>

#include "Criptography.hpp"

using Copy::Security::Cryptography;
using Copy::Security::NO_ENCRYPT_MODE;
using Copy::Security::AES_MODE;

TEST_CASE("AES encryption and description")
{
    Copy::Security::Cryptography<AES_MODE> obj;
        std::string plain_text{"Raw text"};

        SECTION("Encryption test")
        {
            REQUIRE( obj.encrypt(plain_text) != plain_text );
        }
}

TEST_CASE("No encryption and description")
{
        Cryptography<NO_ENCRYPT_MODE> obj;
        std::string plain_text{"Raw text"};

        SECTION("Dummmy test")
        {
            REQUIRE( obj.encrypt(plain_text) == plain_text );
            REQUIRE( obj.decrypt(plain_text) == plain_text );
        }
}
