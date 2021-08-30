#ifndef AMMOPROTOCOL_CHANNEL_HPP
#define AMMOPROTOCOL_CHANNEL_HPP

#include "network.hpp"
namespace ammo::network {
    template<typename T>
    class channel {
    public:
        channel() {

        }
    private:
        ammo::network::receiver<T> receiver_;
        ammo::network::sender<T> sender_;
    };
}

#endif //AMMOPROTOCOL_CHANNEL_HPP
