#include <iostream>
#include <AMMOProtocol.hpp>

enum PacketType: uint32_t {
    PacketFragment,
    Ping
};

class SimpleServer: public ammo::role::server<PacketType> {
public:
    explicit SimpleServer(uint16_t port): ammo::role::server<PacketType>(port) {}

protected:
    void on_message(ammo::common::owned_message<PacketType>& msg) override {
        std::cout << "Received a msg!\n";
        std::cout << "From: " << msg.remote.address() << ':' << msg.remote.port() << "\n";
        std::cout << msg.message.body.data() << std::endl;
        if (msg.message.header.id == Ping) {
            send(msg);
        }
    }
};

int main() {
    SimpleServer server(50000);
    server.start();
    std::atomic_bool updating = true;

    std::thread update_thread([&server, &updating] () {
        while (updating) {
            server.update(64, true, std::chrono::minutes(5));
        }
    });

    while (true) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "/stop") {
            server.stop();
            updating = false;
            server.tick();
            if (update_thread.joinable())
                update_thread.join();
            return 0;
        }
    }
}