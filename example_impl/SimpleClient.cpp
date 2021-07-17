#include <iostream>
#include <AMMOProtocol.hpp>

enum PacketType: uint32_t {
    PacketFragment,
    Ping
};

class SimpleClient: public ammo::role::client<PacketType> {

};

int main() {
    SimpleClient client;
    client.connect("127.0.0.1", 50000);

    bool will_quit = false;
    std::thread thread([&]() {
        if (client.connected()) {

        }
    });

    while (true) {
        int a; std::cin >> a;
        if (a == 1) {
            ammo::common::message<PacketType> msg;
            msg.header.id = Ping;
            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            msg << now;
            msg.pack();
            std::cout << "Sending" << std::endl;
            client.send(msg);
        } else if (a == 2) {
            break;
        }
    }
//    client.disconnect();
    if (thread.joinable())
        thread.join();
}