#include "audeo/Sound.hpp"

namespace audeo {

Sound::Sound() : handle(-1) {}

Sound::Sound(std::int64_t handle) : handle(handle) {}

std::int64_t Sound::value() const { return handle; }

} // namespace audeo
