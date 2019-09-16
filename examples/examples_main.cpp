#include "audeo/audeo.hpp"

#include <iostream>

void basic_playback();
void control_sounds();
void positional_audio();
void effects();

int main() {
    // Initialize audeo.

    audeo::InitInfo info;
    // Reserve some extra channels instead of the default (16). This amount can
    // later be raised by calling audeo::allocate_effect_channels(count).
    info.effect_channels = 32;
    if (!audeo::init(info)) {
        std::cout << "Failed to initialize audeo.\n";
        return -1;
    }

    // Example functions. Please don't call all of these at once if you want to
    // save your ears ;)
    
//	basic_playback();

//	control_sounds();

//	positional_audio();

	effects();

    // Quit audeo
    audeo::quit();
}
