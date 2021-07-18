#ifndef AMMOPROTOCOL_MODULO_QUEUE_HPP
#define AMMOPROTOCOL_MODULO_QUEUE_HPP
#include "structure.hpp"

namespace ammo::structure {
template<typename T, size_t Size>
class modulo_queue {
    std::array<std::optional<T>, Size> entries;
public:
    typedef typename std::array<std::optional<T>, Size>::iterator iterator;
    typedef typename std::array<std::optional<T>, Size>::const_iterator const_iterator;

    iterator begin() { return entries.begin(); }
    iterator end() { return entries.end(); }
    const_iterator begin() const { return entries.begin(); }
    const_iterator end() const { return entries.end(); }
    const_iterator cbegin() const { return entries.cbegin(); }
    const_iterator cend() const { return entries.cend(); }

    std::optional<T>& operator[](size_t raw_index) {
        const size_t index = raw_index % Size;
        return entries[index];
    }

    size_t size() {
        return entries.size();
    }
};

}

#endif //AMMOPROTOCOL_MODULO_QUEUE_HPP
