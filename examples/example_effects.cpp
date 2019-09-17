#include "audeo/audeo.hpp"

void effects() {
    audeo::SoundSource source = audeo::load_source("test_samples/bell.wav",
                              audeo::AudioType::Effect);

    audeo::Sound sound = audeo::play_sound(source, audeo::loop_forever);

    // Currently, audeo only supports a single effect fully:
    audeo::reverse_stereo(sound);

    // There is an experimental implementation for echo/reverb effects, but it
    // does not work well. Contributions are always welcome
	// audeo::add_effect(sound, audeo::Effect::Echo);

    while (true) {
        // Idle loop
    }
}
