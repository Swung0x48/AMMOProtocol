#ifndef AMMOPROTOCOL_RELIABLE_CHANNEL_HPP
#define AMMOPROTOCOL_RELIABLE_CHANNEL_HPP

#include "network.hpp"
#include "channel_type.hpp"
namespace ammo::network {
    template<typename T>
    class reliable_channel: public channel<T> {
    public:
        explicit reliable_channel(event::event_handler& handler):
                channel<T>(handler){}

        /** 
          * @brief Get called on every server loop
          */
        void on_update() override {
            size_t count = 0;
            for (size_t i = last_not_rcvd_; i <= send_sequence_; ++i) {
                if (count > QUEUE_SIZE)
                    break;

                if (sent_queue_[i] != std::nullopt) {
                    ++count;
                    event::connection_send_event e(sent_queue_[i].value());
                    this->event_handler_.emit(e);
                }
            }
        }

        /** 
          * @brief Get called when a new message is received and available to be processed
          * @param msg    The message to be processed
          */
        void on_receive(ammo::common::message<T>& msg) override {
            // Run ack algorithm first
            rcvd_sequence_ = msg.header.sequence;
            received_queue_[msg.header.sequence] = msg;
//            rcvd_sequence_ = msg.header.last_acked;
            ack_packets(msg.header.last_acked, msg.header.ack_bitmap);

#ifdef DEBUG
            std::cout << "[DEBUG] rcvd queue: ";
            for (size_t i = 0; i < received_queue_.size(); ++i) {
                if (received_queue_[i] == std::nullopt)
                    std::cout << '0';
                else
                    std::cout << '1';
            }
            std::cout << std::endl;

            std::cout << "[DEBUG] send queue: ";
            for (size_t i = 0; i < sent_queue_.size(); ++i) {
                if (sent_queue_[i] == std::nullopt)
                    std::cout << '0';
                else
                    std::cout << '1';
            }
            std::cout << std::endl;
#endif

            // Then run base implementation
            channel<T>::on_receive(msg);
        }

    protected:
        void on_send(ammo::common::message<T>& msg) override {
            channel<T>::on_send(msg);

            // Populate the missing part of the packet(especially sequence)
            msg.header.sequence = ++send_sequence_;

            // Then run ack algorithm
            populate_header_ack_on_send(msg);
            sent_queue_[msg.header.sequence] = msg;

#ifdef DEBUG
            std::cout << "[DEBUG] rcvd queue: ";
            for (size_t i = 0; i < received_queue_.size(); ++i) {
                if (received_queue_[i] == std::nullopt)
                    std::cout << '0';
                else
                    std::cout << '1';
            }
            std::cout << std::endl;

            std::cout << "[DEBUG] send queue: ";
            for (size_t i = 0; i < sent_queue_.size(); ++i) {
                if (sent_queue_[i] == std::nullopt)
                    std::cout << '0';
                else
                    std::cout << '1';
            }
            std::cout << std::endl;
#endif
        }

        virtual void on_acked(uint32_t sequence) {
#ifdef DEBUG
            std::cout << "[DEBUG] Acked " << sequence << std::endl;
#endif
        }

    private:
        void populate_header_ack_on_send(ammo::common::message<T>& msg) {
            msg.header.last_acked = rcvd_sequence_;
            msg.header.ack_bitmap = generate_ack_bits(rcvd_sequence_);
            msg.header.send_time = std::chrono::system_clock::now().time_since_epoch().count();
        }

        size_t ack_packets(uint32_t last_acked, uint32_t ack_bitmap) {
            std::bitset<sizeof(ack_bitmap) * CHAR_BIT> bitmap(ack_bitmap);
            for (uint32_t i = 0; i < sizeof(ack_bitmap) * CHAR_BIT; ++i) {
                if (bitmap[i] && sent_queue_[last_acked - i] != std::nullopt) {
                    on_acked(last_acked - i);
                    sent_queue_[last_acked - i] = std::nullopt;
//                    last_not_rcvd_ = common::message<T>::sequence_min(last_not_rcvd_, last_acked - i, QUEUE_SIZE);
                    last_not_rcvd_ = std::min(last_not_rcvd_, last_acked - i);
                }
            }

            return bitmap.count();
        }

        uint32_t generate_ack_bits(uint32_t sequence) {
            std::bitset<sizeof(sequence) * CHAR_BIT> bitmap;
            bitmap.reset();
            for (int i = 0; i < sizeof(uint32_t) * CHAR_BIT; ++i) {
                if (received_queue_[sequence - i] && received_queue_[sequence - i].value().header.sequence == sequence - i)
                    bitmap.set(i);
            }

            return uint32_t(bitmap.to_ulong());
        }

    protected:
        constexpr static size_t QUEUE_SIZE = 1024;

        // tx
        uint32_t send_sequence_ = 0u;
        uint32_t last_not_rcvd_ = 0u;
        ammo::structure::modulo_queue<ammo::common::message<T>, QUEUE_SIZE> sent_queue_;

        // rx
        uint32_t rcvd_sequence_ = 0u;
        ammo::structure::modulo_queue<ammo::common::message<T>, QUEUE_SIZE> received_queue_;

    };
}

#endif //AMMOPROTOCOL_RELIABLE_CHANNEL_HPP
