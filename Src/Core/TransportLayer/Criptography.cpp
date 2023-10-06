//
// Created by daniil on 9/8/23.
//

#include "Criptography.hpp"

#include <cryptopp/files.h>

#include <boost/log/trivial.hpp>
#include <exception>
#include <filesystem>

using CryptoPP::SecByteBlock;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::VectorSource;

std::string Copy::Security::AES_MODE::encryptImp(std::byte* pData, std::size_t size)
{
    std::string source = {reinterpret_cast<const char*>(pData), size};
    std::string cipher;

    SecByteBlock iv(AES::BLOCKSIZE);
    m_randomGenerator.GenerateBlock(iv, iv.size());

    try
    {
        auto key = getKey();
        e.SetKeyWithIV(key, key.size(), iv);

        StringSource strSrc(source, true, new StreamTransformationFilter(e, new StringSink(cipher)));
    }
    catch (const std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Reading problem: " << e.what();
        // todo: resolve this case
    }
    return cipher;
}

std::string Copy::Security::AES_MODE::decryptImp(std::byte* pData, std::size_t size)
{
    std::string source = {reinterpret_cast<const char*>(pData), size};
    std::string recovered;
    SecByteBlock iv(AES::BLOCKSIZE);

    m_randomGenerator.GenerateBlock(iv, iv.size());

    try
    {
        CBC_Mode<AES>::Decryption d;
        auto key = getKey();
        d.SetKeyWithIV(key, key.size(), iv);
        StringSource s(source, true, new StreamTransformationFilter(d, new StringSink(recovered)));
    }
    catch (const std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Reading problem: " << e.what();
        // todo: resolve this case
    }
    return recovered;
}

CryptoPP::SecByteBlock Copy::Security::AES_MODE::getKey()
{
    std::string file_content;
    try
    {
        std::filesystem::path AES_KEY_PATH({std::getenv("AES_KEY_PATH"), AES::DEFAULT_KEYLENGTH});

        std::ifstream file(AES_KEY_PATH, std::ifstream::binary);
        CryptoPP::SecByteBlock key(file.tellg());

        file.seekg(0, std::ios_base::end);
        key.resize(file.tellg() - file.tellg());

        file.seekg(0, std::ios_base::beg);
        file.read((char*)key.begin(), key.size());
        return key;
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Reading problem: " << e.what();
    }

    return CryptoPP::SecByteBlock();
}
