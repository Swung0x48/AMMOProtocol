#ifndef AMMOPROTOCOL_STRING_HPP
#define AMMOPROTOCOL_STRING_HPP
#include "entity.hpp"
namespace ammo::entity {
    template <typename T>
    struct string: public ammo::entity::entity<T> {
        std::string str;
    public:
        string<T>() = default;
        string<T>(std::string& s): str(s) {}
        string<T>(const char* s): str(s) {}
        void serialize(ammo::common::message<T>& packet) override {
            uint64_t length = str.length();
            packet << length;
            packet.write(str.data(), str.length());
        }
        void deserialize(ammo::common::message<T>& packet) override {
            uint64_t length; packet >> length;
            str.resize(length);
            packet.read(str.data(), str.length());
        }
    };
}
#endif //AMMOPROTOCOL_STRING_HPP
