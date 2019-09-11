#include "audeo/audeo.hpp"
#include <iostream>
#include <string>

int main() {
    try {

        audeo::SoundEngine::InitInfo info;
        info.output_channels = audeo::SoundEngine::OutputChannelCount::Stereo;
        audeo::SoundEngine engine(info);

		while (engine.is_playing_music()) {}

    } catch (audeo::exception const& e) { std::cout << e.what(); }

    return 0;
}
