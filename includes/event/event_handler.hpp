#ifndef AMMOPROTOCOL_EVENT_HANDLER_HPP
#define AMMOPROTOCOL_EVENT_HANDLER_HPP

#include <unordered_map>
#include <functional>
#include <memory>
#include <typeinfo>
#include <typeindex>

namespace ammo::event {
    class event_handler {
    private:
        class basic_listener {
        public:
            explicit basic_listener() = default;
            basic_listener(const basic_listener&) = delete;
            basic_listener& operator=(const basic_listener&) = delete;
            virtual ~basic_listener() = default;
        };

        template<typename Event>
        class listener : public basic_listener {
        public:
            explicit listener(const std::function<void(Event&)>& callback) {
                this->callback = callback;
            }

            listener(const listener&) = delete;
            listener& operator=(const listener&) = delete;
            ~listener() override = default;

            void emit(Event& type) {
                try {
                    callback(type);
                } catch (...) {
                }
            }

        private:
            std::function<void(Event&)> callback;
        };

    public:
        template<typename Event>
        void emit(Event& type) {
            auto it = listeners.find(typeid(type));
            if (it != listeners.end())
                static_cast<listener<Event>*>(it->second.get())->emit(type);
        }

        template<typename Event>
        event_handler& on(const std::function<void(Event&)>& callback) {
            listeners[typeid(Event)] = std::unique_ptr<basic_listener>(new listener<Event>(callback));
            return *this;
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<basic_listener>> listeners;
    };
}

#endif //AMMOPROTOCOL_EVENT_HANDLER_HPP
