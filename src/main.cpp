#include "audeo/audeo.hpp"
#include <iostream>
#include <string>

int main() {
    try {
        audeo::SoundEngine::InitInfo info;
        info.output_channels = audeo::OutputChannelCount::Stereo;
        audeo::SoundEngine engine(info);

        audeo::SoundSource music("test_samples/happy_music.mp3",
                                 audeo::AudioType::Music);
        engine.play_music(music, audeo::loop_forever);
        while (engine.is_playing_music()) {
			// Finish the track
		}

    } catch (audeo::exception const& e) { std::cout << e.what(); }

    return 0;
}
