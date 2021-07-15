#ifndef AMMOPROTOCOL_VEC3_HPP
#define AMMOPROTOCOL_VEC3_HPP
#include "entity.hpp"
namespace ammo::entity {
    template<typename T>
    struct vec3: public ammo::entity::entity<T> {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    public:
        void serialize(ammo::common::message<T>& packet) override {
            packet << x << y << z;
        }
        void deserialize(ammo::common::message<T>& packet) override {
            packet >> x >> y >> z;
        }
    };
}

#endif //AMMOPROTOCOL_VEC3_HPP
