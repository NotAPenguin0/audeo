#ifndef AUDEO_SOUND_HPP_
#define AUDEO_SOUND_HPP_

#include <cstdint>
#include <utility>
#include <functional>

namespace audeo {

class Sound {
public:
    Sound() : handle(-1) {}
    Sound(std::int64_t handle) : handle(handle) {}
    Sound(Sound const&) = default;
    Sound(Sound&&) = default;

    Sound& operator=(Sound const&) = default;
    Sound& operator=(Sound&&) = default;

    std::int64_t value() const { return handle; }

    bool operator==(Sound const& rhs) const { return handle == rhs.handle; }
    bool operator!=(Sound const& rhs) const { return handle != rhs.handle; }

private:
    std::int64_t handle;
};

} // namespace audeo

namespace std {
template<>
struct hash<audeo::Sound> {
    size_t operator()(audeo::Sound const& x) const {
        return hash<std::int64_t>()(x.value());
    }
};
} // namespace std

#endif
