#ifndef AMMOPROTOCOL_ENTITY_HPP
#define AMMOPROTOCOL_ENTITY_HPP

namespace ammo::entity {
    template<typename T>
    class entity {
    public:
        virtual void serialize(ammo::common::message<T>& packet) = 0;
        virtual void deserialize(ammo::common::message<T>& packet) = 0;
    };
}


#endif //AMMOPROTOCOL_ENTITY_HPP
