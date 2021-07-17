#include <iostream>
#include <AMMOProtocol.hpp>
using namespace std;

enum PacketType: uint32_t {
    PacketFragment,
    Echo
};

class SimpleServer: public ammo::role::server<PacketType> {
public:
    explicit SimpleServer(uint16_t port): ammo::role::server<PacketType>(port) {}

protected:
    void on_message(ammo::common::owned_message<PacketType>& msg) override {
        std::cout << "Received a msg!\n";
        std::cout << "From: " << msg.remote.address() << ':' << msg.remote.port() << "\n";
        std::cout << msg.message.body.data() << std::endl;
    }
};

int main() {
    SimpleServer server(50000);
    server.start();

    while (true) {
        server.update(64);
    }
}