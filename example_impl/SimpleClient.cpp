#include <iostream>
#include <AMMOProtocol.hpp>

enum PacketType: uint32_t {
    PacketFragment,
    Ping,
    Name
};

class SimpleClient: public ammo::role::client<PacketType> {
public:
    void send_validation() override {
        ammo::common::message<PacketType> msg;
        ammo::entity::string<PacketType> name = "Test";
        msg.header.id = Name;
        name.serialize(msg);
        send(msg);
    }
};

int main() {
    SimpleClient client;
    if (client.connect("127.0.0.1", 50000))
        std::cout << "[INFO] Connecting to server..." << std::endl;

    std::atomic_bool will_quit = false;
    std::thread thread([&]() {
        while (!will_quit) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            if (client.get_state() == ammo::role::client_state::Pending) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (client.get_incoming_messages().empty())
                    client.send_validation();
                else {
                    auto msg = client.get_incoming_messages().pop_front();
                    if (msg.message.header.id == Name) {
                        client.confirm_validation();
                        std::cout << "[INFO] Connected to server!" << std::endl;
                    }
                }

            }
            if (client.connected()) {
                if (!client.get_incoming_messages().empty()) {
                    auto msg = client.get_incoming_messages().pop_front();
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
                            break;
                        }
                        default: {
                            std::cout << "[WARN] Unknown packet type: " << msg.message.header.id << std::endl;
                            break;
                        }
                    }
                }
            }
        }
    });

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
            will_quit = true;
            break;
        }
    }
    client.disconnect();
    if (thread.joinable())
        thread.join();
}