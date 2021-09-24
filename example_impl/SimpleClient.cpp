#include <iostream>
#include <AMMOProtocol.hpp>

enum PacketType: uint32_t {
    PacketFragment,
    Ping,
    Name,
    Count
};

class SimpleClient: public ammo::role::client<PacketType> {
public:
    void send_request() override {
        ammo::common::message<PacketType> msg;
        ammo::entity::string<PacketType> name = "Test";
        msg.header.id = Name;
        name.serialize(msg);
        ammo::common::owned_message<PacketType> omsg {server_endpoint_, msg};
        commit_send(omsg);
    }

    void on_message(ammo::network::connection<PacketType>& destination, ammo::common::message<PacketType>& msg) override {
        switch (msg.header.id) {
            case PacketType::PacketFragment: {
                break;
            }
            case PacketType::Ping: {
                auto now = std::chrono::system_clock::now().time_since_epoch().count();
                uint64_t then; msg >> then;
                auto ping = (now - then) / 1000;
                std::cout << msg.header.sequence << std::endl;
                std::cout << msg.header.last_acked << std::endl;
                std::cout << std::bitset<sizeof(msg.header.ack_bitmap) * CHAR_BIT>(msg.header.ack_bitmap) << std::endl;
                std::cout << "[INFO] Ping: " << ping << " ms" << std::endl;
                break;
            }
            case PacketType::Count: {

            }
            default: {
                std::cout << "[WARN] Unknown packet type: " << msg.header.id << std::endl;
                break;
            }
        }
    }

protected:
    void on_authenticate_message(ammo::common::owned_message<PacketType>& msg) override {
        switch (msg.message.header.id) {
            case PacketType::PacketFragment: {
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

    void on_update() override {

    }
};

int main() {
    SimpleClient client;
    if (client.connect("127.0.0.1", 50000))
        std::cout << "[INFO] Connecting to server..." << std::endl;

    while (true) {
        int a; std::cin >> a;
        if (a == 1) {
            for (int i = 0; i < 50; ++i) {
                ammo::common::message<PacketType> msg;
                msg.header.id = Count;
                msg << i;
                msg.set_reliable(true);
                client.send(client.get_server_connection(), msg);
            }
        } else if (a == 2) {
            ammo::common::message<PacketType> msg;
            msg.header.id = Ping;
            auto now = std::chrono::system_clock::now().time_since_epoch().count();
            msg << now;
            msg.set_reliable(true);
            client.send(client.get_server_connection(), msg);
        }  else {
            break;
        }
    }
}