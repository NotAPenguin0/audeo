#ifndef AUDEO_SOUND_SOURCE_HPP_
#define AUDEO_SOUND_SOURCE_HPP_

#include <SDL_mixer.h>
#include <string_view>

#include "export_import.hpp"
#include "exception.hpp"
#include "vec3.hpp"

namespace audeo {

enum class AudioType { Music, Effect };

class SoundSource {
public:
    SoundSource() : handle(-1) {}
    SoundSource(std::int64_t handle) : handle(handle) {}
    SoundSource(SoundSource const&) = default;
    SoundSource(SoundSource&&) = default;

    SoundSource& operator=(SoundSource const&) = default;
    SoundSource& operator=(SoundSource&&) = default;
    std::int64_t value() const { return handle; }

    bool operator==(SoundSource const& rhs) const {
        return handle == rhs.handle;
    }
    bool operator!=(SoundSource const& rhs) const {
        return handle != rhs.handle;
    }

private:
    std::int64_t handle;
};

} // namespace audeo

namespace std {
template<>
struct hash<audeo::SoundSource> {
    size_t operator()(audeo::SoundSource const& x) const {
        return hash<std::int64_t>()(x.value());
    }
};
} // namespace std

#endif
