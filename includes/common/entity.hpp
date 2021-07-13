#ifndef AMMOPROTOCOL_ENTITY_HPP
#define AMMOPROTOCOL_ENTITY_HPP
#include "common.hpp"
#include "packet.hpp"

namespace ammo::common {
    template<typename T>
    class entity {
    public:
        entity() = default;
        entity(ammo::common::packet<T>& packet) {
            deserialize(packet);
        }

        virtual void serialize(ammo::common::packet<T>& packet) = 0;
        virtual void deserialize(ammo::common::packet<T>& packet) = 0;
    };
}


#endif //AMMOPROTOCOL_ENTITY_HPP
