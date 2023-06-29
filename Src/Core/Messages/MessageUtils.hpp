//
// Created by daniil on 6/27/23.
//

#ifndef EPAMMIDDLE_MESSAGEUTILS_HPP
#define EPAMMIDDLE_MESSAGEUTILS_HPP

#include <google/protobuf/message_lite.h>

#include <concepts>

namespace Clone::Utils
{
static constexpr std::uint32_t start_dword = 0x0AA00AA0;
static constexpr std::uint32_t end_dword   = 0x05500550;

template <typename T>
concept ProtobufMessageDerived = std::derived_from<T, ::google::protobuf::MessageLite>;

template <class ContainerType>
concept Container = requires(ContainerType a, const ContainerType b) { requires std::regular<ContainerType>; };

template <ProtobufMessageDerived ProtobufMessage_t>
inline std::shared_ptr<std::vector<char>> MessageToVector(ProtobufMessage_t& message)
{
    auto buffer = std::make_shared<std::vector<char>>();
    //-----------------------------------
    auto write = [&buffer](auto data) {
        std::copy(reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data) + sizeof(data), std::back_inserter(*buffer));
    };
    auto writeC = [&buffer](auto&& data) { std::copy(std::begin(data), std::end(data), std::back_inserter(*buffer)); };
    //-----------------------------------
    write(start_dword);
    std::string m = message.SerializeAsString();
    auto size     = static_cast<std::uint64_t>(m.size());
    write(size);
    writeC(m);
    write(end_dword);
    return buffer;
}

template <ProtobufMessageDerived ProtobufMessage_t, Container container_t>
inline std::unique_ptr<ProtobufMessage_t> parseProtobufMsg(const container_t& container)
{
    auto header_size{sizeof(std::uint32_t) + sizeof(std::uint64_t)};
    auto pBegin = container.data();
    std::uint64_t message_size{};
    memcpy(&message_size, pBegin + sizeof(std::uint32_t), sizeof(std::uint64_t));
    if (auto msg_p = std::make_unique<ProtobufMessage_t>(); msg_p->ParseFromArray(pBegin + header_size, message_size)) return msg_p;
    return {nullptr};
}

};      // namespace Clone::Utils
#endif  // EPAMMIDDLE_MESSAGEUTILS_HPP
