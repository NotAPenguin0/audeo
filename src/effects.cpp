#include "audeo/effects.hpp"

#include <SDL_mixer.h>

#include <cmath>
#include <iostream>

namespace audeo {

void echo_callback(int channel, void* stream, int length, void* user_data) {
    auto* data = reinterpret_cast<std::int16_t*>(stream);
    int bufsize = length / sizeof(std::int16_t);

    constexpr int ms_delay = 300;
    constexpr int delay = static_cast<int>(ms_delay * 44.1f);
    constexpr float decay = 0.5f;

    // Simple echo effect. To create true reverb, we have to run this in reverse
    for (int i = bufsize - delay - 1; i >= 0; --i) {
        data[i + delay] += static_cast<std::int16_t>(data[i] * decay);
    }
}

} // namespace audeo
