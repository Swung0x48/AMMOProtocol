#ifndef AMMOPROTOCOL_TS_QUEUE_HPP
#define AMMOPROTOCOL_TS_QUEUE_HPP
#include "structure.hpp"

namespace ammo::structure {
    template<typename T>
    class ts_queue {
    public:
        ts_queue() = default;

        ts_queue(const ts_queue<T>&) = delete; // explicitly delete copy constructor
        virtual ~ts_queue() { clear(); }

    protected:
        std::mutex queue_mtx_;
        std::deque <T> raw_deque_;
        std::condition_variable to_update_cv_;
        std::mutex to_update_mtx_;
        std::atomic_bool do_tick = false;

    public:
        const T &front() {
            std::scoped_lock lock(queue_mtx_);
            return raw_deque_.front();
        }

        const T &back() {
            std::scoped_lock lock(queue_mtx_);
            return raw_deque_.back();
        }

        void push_front(const T &item) {
            std::scoped_lock lock(queue_mtx_);
            raw_deque_.emplace_front(std::move(item));

            std::unique_lock <std::mutex> lk(to_update_mtx_);
            to_update_cv_.notify_one();
        }

        void push_back(const T &item) {
            std::scoped_lock lock(queue_mtx_);
            raw_deque_.emplace_back(std::move(item));

            std::unique_lock <std::mutex> lk(to_update_mtx_);
            to_update_cv_.notify_one();
        }

        bool empty() {
            std::scoped_lock lock(queue_mtx_);
            return raw_deque_.empty();
        }

        size_t size() {
            std::scoped_lock lock(queue_mtx_);
            return raw_deque_.size();
        }

        void clear() {
            std::scoped_lock lock(queue_mtx_);
            raw_deque_.clear();
        }

        T pop_front() {
            std::scoped_lock lock(queue_mtx_);
            auto t = std::move(raw_deque_.front());
            raw_deque_.pop_front();
            return t;
        }

        T pop_back() {
            std::scoped_lock lock(queue_mtx_);
            auto t = std::move(raw_deque_.back());
            raw_deque_.pop_back();
            return t;
        }

        void wait() {
            while (empty() && !do_tick) {
                std::unique_lock <std::mutex> lk(to_update_mtx_);
                to_update_cv_.wait(lk);
            }
            do_tick = false;
        }

        template<class Rep, class Period>
        std::cv_status wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
            std::cv_status status;
            while (empty() && !do_tick) {
                std::unique_lock <std::mutex> lk(to_update_mtx_);
                status = to_update_cv_.wait_for(lk, rel_time);
            }
            do_tick = false;
            return status;
        }

        void tick() {
            do_tick = true;
            to_update_cv_.notify_one();
        }
    };
}
#endif //AMMOPROTOCOL_TS_QUEUE_HPP
