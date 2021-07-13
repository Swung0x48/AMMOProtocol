#ifndef AMMOPROTOCOL_VEC3_HPP
#define AMMOPROTOCOL_VEC3_HPP
#include "entity.hpp"
namespace ammo::entities {
    template<typename T>
    struct vec3: public ammo::common::entity<T> {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    public:
        void serialize(ammo::common::packet<T>& packet) override {
            packet.write(x);
            packet.write(y);
            packet.write(z);
        }
        void deserialize(ammo::common::packet<T>& packet) override {
            packet.read(x);
            packet.read(y);
            packet.read(z);
        }
    };
}

#endif //AMMOPROTOCOL_VEC3_HPP
