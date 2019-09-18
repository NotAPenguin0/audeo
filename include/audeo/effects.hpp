#ifndef AUDEO_EFFECTS_HPP_
#define AUDEO_EFFECTS_HPP_

#include "audeo/SoundEngine.hpp"

namespace audeo {

AUDEO_API void
echo_callback(int channel, void* stream, int length, void* user_data);

} // namespace audeo

#endif
