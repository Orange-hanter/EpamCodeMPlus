//
// Created by daniil on 6/27/23.
// leave a
//

#ifndef EPAMMIDDLE_MESSAGEUTILS_HPP
#define EPAMMIDDLE_MESSAGEUTILS_HPP

#include <google/protobuf/message_lite.h>

#include <concepts>

namespace Clone::Utils
{
static constexpr std::uint32_t start_dword = 0x05500550;
static constexpr std::uint32_t end_dword   = 0x0AA00AA0;

template <typename T>
concept ProtobufMessageDerived = std::derived_from<T, ::google::protobuf::MessageLite>;

template <class ContainerType>
concept Container = requires(ContainerType a, const ContainerType b) { requires std::regular<ContainerType>; };

template <ProtobufMessageDerived ProtobufMessage_t>
inline std::shared_ptr<std::vector<char>> messageToPackage(ProtobufMessage_t& message, uint32_t mask = 0)
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
    size |= (static_cast<std::uint64_t>(mask) << 32) & 0xFFFFFFFF00000000;
    write(size);
    writeC(m);
    write(end_dword);
    BOOST_LOG_TRIVIAL(trace) << "Send " << buffer->size() << " bytes";
    return buffer;
}

template <ProtobufMessageDerived ProtobufMessage_t, Container container_t>
inline std::unique_ptr<ProtobufMessage_t> packageToMessage(const container_t& container)
{
    auto header_size{sizeof(std::uint32_t) + sizeof(std::uint64_t)};
    auto data_ptr = container.data();
    std::uint64_t message_size{};
    memcpy(&message_size, data_ptr + sizeof(std::uint32_t), sizeof(std::uint64_t));
    message_size &= 0x00000000FFFFFFFF;
    if (auto msg_p = std::make_unique<ProtobufMessage_t>(); msg_p->ParseFromArray(data_ptr + header_size, message_size)) return msg_p;
    BOOST_LOG_TRIVIAL(error) << "Serialization problem: Protobuf package corrupted";
    return {nullptr};
}

template <typename T>  // TODO work around non pointer type
inline std::size_t receiverAlgorithm(T* data_ptr, std::size_t received)
{
    constexpr auto header_size{sizeof(std::uint32_t) + sizeof(std::uint64_t)};
    BOOST_LOG_TRIVIAL(trace) << "\t\tReceived " << received << " bytes";
    if (received < header_size) return (header_size - received);

    std::uint32_t package_starter{*reinterpret_cast<std::uint32_t*>(data_ptr)};
    if (package_starter == Utils::start_dword)
    {
        auto message_size   = *reinterpret_cast<std::uint64_t*>(data_ptr + sizeof(std::uint32_t)) & 0x00000000FFFFFFFF;
        auto estimated_size = message_size + sizeof(Utils::end_dword) - (received - header_size);
        if (estimated_size != 0)
        {
            BOOST_LOG_TRIVIAL(trace) << "\tEstimated " << estimated_size << " bytes";
            return estimated_size;  // wait such bytes before return
        }
        auto end_ptr_shift = header_size + message_size;

        if ((*reinterpret_cast<std::uint32_t*>(data_ptr + end_ptr_shift)) == Utils::end_dword)
        {
            std::fill_n(data_ptr, sizeof(start_dword), 0);
            data_ptr[end_ptr_shift] = 0;
            BOOST_LOG_TRIVIAL(trace) << "\tPackage received (" << received << " bytes)";
            return 0UL;
        }
    }
    BOOST_LOG_TRIVIAL(error) << "Reading problem: Protocol sequence violation";
    return 0UL;  // TODO somehow we need indicate about error
}

template <typename T>
inline auto getFlags(T* data_ptr)
{
    auto value = *reinterpret_cast<std::uint32_t*>(data_ptr + sizeof(std::uint32_t) * 2);
    return value;
}

};      // namespace Clone::Utils
#endif  // EPAMMIDDLE_MESSAGEUTILS_HPP
