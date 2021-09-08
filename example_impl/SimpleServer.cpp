#include <iostream>
#include <AMMOProtocol.hpp>
#include <atomic>

enum PacketType: uint32_t {
    PacketFragment,
    Ping,
    Name
};

class SimpleServer: public ammo::role::server<PacketType> {
public:
    explicit SimpleServer(uint16_t port): ammo::role::server<PacketType>(port) {}

protected:
    void on_message(ammo::network::connection<PacketType>& destination, ammo::common::message<PacketType>& msg) override {
        switch (msg.header.id) {
            case PacketFragment:
                break;
            case Ping:
                std::cout << "Rcvd a ping" << std::endl;
                send(destination, msg);
                break;
            case Name:
                ammo::entity::string<PacketType> name;
                name.deserialize(msg);
                std::cout << "Client identify itself as " << name.str << std::endl;
                send(destination, msg);
                break;
        }
    }

    void on_authenticate_message(ammo::common::owned_message<PacketType>& msg) override {

    }
};

void do_idle_work() {
    // Do something here when the server is idle. (Regular cleanup/checkup etc.)
}

int main() {
    SimpleServer server(50000);
    server.start();
    std::atomic_bool updating = true;

    std::thread update_thread([&server, &updating] () {
        while (updating) {
            auto status = server.update(64, true, std::chrono::minutes(5));
            if (status == std::cv_status::timeout) {
                do_idle_work();
            }
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