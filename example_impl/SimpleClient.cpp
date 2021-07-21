#include <iostream>
#include <AMMOProtocol.hpp>

enum PacketType: uint32_t {
    PacketFragment,
    Ping,
    Name
};

class SimpleClient: public ammo::role::client<PacketType> {
public:
    void send_request() override {
        ammo::common::message<PacketType> msg;
        ammo::entity::string<PacketType> name = "Test";
        msg.header.id = Name;
        name.serialize(msg);
        send(msg);
    }

    void on_message(ammo::common::owned_message<PacketType>& msg) override {
        switch (msg.message.header.id) {
            case PacketType::PacketFragment: {
                break;
            }
            case PacketType::Ping: {
                auto now = std::chrono::system_clock::now().time_since_epoch().count();
                uint64_t then; msg.message >> then;
                auto ping = (now - then) / 1000;
                std::cout << "[INFO] Ping: " << ping << " ms" << std::endl;
                break;
            }
            case PacketType::Name: {
                confirm_validation();
                std::cout << "[INFO] Connected to server!" << std::endl;
                break;
            }
            default: {
                std::cout << "[WARN] Unknown packet type: " << msg.message.header.id << std::endl;
                break;
            }
        }
    }
};

int main() {
    SimpleClient client;
    if (client.connect("127.0.0.1", 50000))
        std::cout << "[INFO] Connecting to server..." << std::endl;

    client.send_request();

    while (true) {
        int a; std::cin >> a;
        if (a == 1) {
            ammo::common::message<PacketType> msg;
            msg.header.id = Ping;
            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            msg << now;
//            msg.pack();
            client.send(msg);
        } else if (a == 2) {
            break;
        }
    }
    client.shutdown();
}