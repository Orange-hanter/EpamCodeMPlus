//
// Created by daniil on 9/8/23.
//

#ifndef EPAMMIDDLE_CRIPTOGRAPHY_HPP
#define EPAMMIDDLE_CRIPTOGRAPHY_HPP

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>

#include <cstdlib>
#include <string>

namespace Copy::Security
{
using CryptoPP::AES;
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::CBC_Mode;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::StringSink;

template <typename Derived_t>
class Cryptography
{
    Derived_t* self() { return static_cast<Derived_t*>(this); }

public:
    std::string encrypt(std::string plain) { return self()->encryptImp(reinterpret_cast<std::byte*>( plain.data()), plain.length()); }
    std::string encrypt(std::byte* pData, std::size_t size) { return self()->encryptImp(pData, size); }

    std::string decrypt(std::string plain) { return self()->decryptImp(plain); }
    std::string decrypt(std::byte* pData, std::size_t size) { return self()->decryptImp(pData, size); }
};

class NO_ENCRYPT_MODE : public Cryptography<NO_ENCRYPT_MODE>
{
public:
    std::string encryptImp(std::string plain) { return plain; }
    std::string decryptImp(std::string plain) { return plain; }
    std::string encryptImp(std::byte* pData, std::size_t size) { return {reinterpret_cast<const char*>(pData), size}; }
    std::string decryptImp(std::byte* pData, std::size_t size) { return {reinterpret_cast<const char*>(pData), size}; }
};

class AES_MODE : public Cryptography<AES_MODE>
{
    CBC_Mode<AES>::Encryption e;
    AutoSeededRandomPool m_randomGenerator;  // for init vector generation
    CryptoPP::SecByteBlock getKey();

public:
    std::string encryptImp(std::byte* pData, std::size_t size);
    std::string decryptImp(std::byte* pData, std::size_t size);

};

}  // end namespace

#endif  // EPAMMIDDLE_CRIPTOGRAPHY_HPP
