#include <iostream>
#include <AMMOProtocol.hpp>
#include <atomic>

enum PacketType: uint32_t {
    PacketFragment,
    Ping,
    Name,
    Count
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
                std::cout << msg.header.sequence << std::endl;
                std::cout << msg.header.last_acked << std::endl;
                std::cout << std::bitset<sizeof(msg.header.ack_bitmap) * CHAR_BIT>(msg.header.ack_bitmap) << std::endl;
                send(destination, msg);
                break;
            case PacketType::Count: {
                int tmp; msg >> tmp;
                count_.emplace_back(tmp);
            }
            default:
                break;
        }
    }

    void on_authenticate_message(ammo::common::owned_message<PacketType>& msg) override {
        switch (msg.message.header.id) {
            case PacketFragment:
                break;
            case Name: {
                ammo::entity::string<PacketType> name;
                name.deserialize(msg.message);
                std::cout << "Client identify itself as " << name.str << std::endl;
                send(accept_connection(msg.remote), msg.message);
                break;
            }
            default:
                break;
        }
    }

    void on_update() override {

    }

public:
    std::vector<int> count_;
};

int main() {
    SimpleServer server(50000);
    server.start();

#ifdef DEBUG
    std::cout << "[DEBUG] Now running in debug mode." << std::endl;
#endif

    std::cout << "[INFO] Server started!" << std::endl;

    while (true) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "/stop") {
            server.stop();
            server.tick();
            return 0;
        } else if (cmd == "/show") {
            std::cout << (server.count_.size() == 50) << std::endl;
            for (auto i: server.count_) {
                std::cout << i << ' ';
            }
            std::cout << std::endl;
        } else if (cmd == "/clear") {
            server.count_.clear();
        }
    }
}