#ifndef AMMOPROTOCOL_CLIENT_STATE_HPP
#define AMMOPROTOCOL_CLIENT_STATE_HPP

namespace ammo::role {
    enum class client_state {
        Disconnected,
        Pending,
        Connected
    };
}

#endif //AMMOPROTOCOL_CLIENT_STATE_HPP
