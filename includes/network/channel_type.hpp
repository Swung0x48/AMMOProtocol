#ifndef AMMOPROTOCOL_CHANNEL_TYPE_HPP
#define AMMOPROTOCOL_CHANNEL_TYPE_HPP

namespace ammo::network {
    enum class channel_type {
        Unreliable,
        Reliable,
        ReliableOrdered
    };
}

#endif //AMMOPROTOCOL_CHANNEL_TYPE_HPP
