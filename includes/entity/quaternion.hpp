#ifndef AMMOPROTOCOL_QUATERNIONS_HPP
#define AMMOPROTOCOL_QUATERNIONS_HPP
#include "entity.hpp"
namespace ammo::entity {
    template<typename T>
    struct quaternion: public ammo::entity::entity<T> {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    public:
        void serialize(ammo::common::message<T>& packet) override {
            packet << x << y << z << w;
        }
        void deserialize(ammo::common::message<T>& packet) override {
            packet >> x >> y >> z >> w;
        }
    };
}

#endif //AMMOPROTOCOL_QUATERNIONS_HPP
