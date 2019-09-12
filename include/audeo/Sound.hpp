#ifndef AUDEO_SOUND_HPP_
#define AUDEO_SOUND_HPP_

#include <cstdint>
#include <utility>

namespace audeo {

class Sound {
public:
	Sound();
    Sound(std::int64_t handle);
    Sound(Sound const&) = default;
    Sound(Sound&&) = default;

    Sound& operator=(Sound const&) = default;
    Sound& operator=(Sound&&) = default;

    std::int64_t value() const;

    bool operator==(Sound const& rhs) const { return handle == rhs.handle; }
    bool operator!=(Sound const& rhs) const { return handle != rhs.handle; }

    operator std::int64_t() const { return handle; }

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
