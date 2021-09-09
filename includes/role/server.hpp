#ifndef AMMOPROTOCOL_SERVER_HPP
#define AMMOPROTOCOL_SERVER_HPP

#include "event/event_all.hpp"
#include "role.hpp"
namespace ammo::role {
    template <typename T>
    class server: public role<T> {
    public:
        explicit server(uint16_t port): role<T>(port) {}

        virtual ~server() {
            role<T>::stop();
        }

        void tick() {
            role<T>::receiver_.get_incoming_messages().tick();
        }
    };
}

#endif //AMMOPROTOCOL_SERVER_HPP
