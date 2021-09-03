#ifndef AMMOPROTOCOL_RELIABLE_CHANNEL_HPP
#define AMMOPROTOCOL_RELIABLE_CHANNEL_HPP

#include "network.hpp"
#include <bitset>
namespace ammo::network {
    template<typename T>
    class reliable_channel: public channel<T> {
    public:
    protected:
        void on_send(ammo::common::owned_message<T>& msg) override {
            // Populate the missing part of the packet(especially sequence)
            channel<T>::on_send(msg);

            // Then run ack algorithm
            sent_acked_[msg.message.sequence] = false;
            msg.message.last_acked = last_acked_;
            msg.message.ack_bitmap = generate_ack_bits(msg.message.sequence);
            msg.message.send_time = std::chrono::system_clock::now().time_since_epoch().count();
        }

        void on_receive(ammo::common::owned_message<T>& msg) override {
            // Run ack algorithm first
            last_acked_ = msg.message.sequence_max(last_acked_, msg.message.header.last_acked);
            received_queue_[msg.message.header.sequence] = msg;
            auto ack_bitmap = std::bitset<sizeof(msg.message.header.ack_bitmap) * CHAR_BIT>(msg.message.header.ack_bitmap);
            ack_bitmap.set(0); // Current packet must be received.
//            for (int i = 0; i < bits.size(); ++i) {
//                int position = bits.size() - 1 - i;
//                int seq = msg.message.header.sequence - position - 1;
//                if (bits[position] && (sent_acked_[seq] == std::nullopt || !sent_acked_[seq])) {
//                    on_acked(msg.message.header.sequence - position - 1);
//                }
//            }
            for (size_t i = 0; i < ack_bitmap.size(); ++i) {
                int position = msg.message.header.sequence - i;
                if (ack_bitmap[position] && sent_acked_[position] == std::nullopt || !sent_acked_[position])
                    on_acked(position);
            }

            // Then pass to user implementation
            channel<T>::on_message(msg);
        }

        void on_acked(uint32_t sequence) {
            sent_acked_[sequence] = true;
        }

        uint32_t generate_ack_bits(uint32_t sequence) {
            std::bitset<sizeof(sequence) * CHAR_BIT> bitmap;
            bitmap.reset();
            for (int i = 0; i < sizeof(sequence); ++i) {
                if (sent_acked_[sequence - i] == std::nullopt ? false : sent_acked_[sequence - i])
                    bitmap.set(sequence - i);
            }
//            for (int i = sizeof(sequence) + 1; i >= 1; --i) {
//                ret |= uint32_t(sent_acked_[sequence - i] == std::nullopt ? false : sent_acked_[sequence - i]);
//                ret <<= 1;
//            }
            return uint32_t(bitmap.to_ulong());
        }

        constexpr static size_t QUEUE_SIZE = 1024;
        uint32_t last_acked_ = -1;
        ammo::structure::modulo_queue<bool, QUEUE_SIZE> sent_acked_;
        ammo::structure::modulo_queue<ammo::common::owned_message<T>&, QUEUE_SIZE> received_queue_;
    };
}

#endif //AMMOPROTOCOL_RELIABLE_CHANNEL_HPP
