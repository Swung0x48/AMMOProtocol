#ifndef AMMOPROTOCOL_QUATERNIONS_HPP
#define AMMOPROTOCOL_QUATERNIONS_HPP
#include "entity.hpp"
namespace ammo::entities {
    template<typename T>
    struct quaternion: public ammo::common::entity<T> {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    public:
        void serialize(ammo::common::packet<T>& packet) override {
            packet.write(x);
            packet.write(y);
            packet.write(z);
            packet.write(w);
        }
        void deserialize(ammo::common::packet<T>& packet) override {
            packet.read(x);
            packet.read(y);
            packet.read(z);
            packet.read(w);
        }
    };
}

#endif //AMMOPROTOCOL_QUATERNIONS_HPP
