#include "audeo/audeo.hpp"

// We will also use a small SDL app for this example

#include <SDL.h>

// These functions are all defined in another example file, but we can use them
// over here
SDL_Window* init_sdl();
void quit_sdl(SDL_Window* window);
// Utility function to check for sdl key events
bool sdl_key_event(SDL_Event const& event, int key);

void positional_audio() {

    SDL_Window* window = init_sdl();

    // Load our music source. This time, we will load our music as an effect so
    // that we can apply positional audio functionality to it. Note that this is
    // *not* recommended as a way to do positional music. It is just done here
    // for demonstration purposes so we can have a long lasting sound 'effect'
    audeo::SoundSource source = audeo::load_source("test_samples/happy_music.mp3",
                              audeo::AudioType::Effect);

    // You can control the default parameters a sound will get when playing from
    // a source by calling the following functions on the sound source:
    // source.set_default_volume(default_volume);
    // source.set_default_position(default_position)

    audeo::Sound sound = audeo::play_sound(source, audeo::loop_forever);

    // This will map the our own distance range for the sound (0-100 units) to
    // the default distance range (0-255 units). It is advised that you tweak
    // this value to adjust it to the 'scale' of your application (if 100 units
    // is ~1 meter in your app you will need a larger distance range). Another
    // way of looking at this value is the maximum distance in units
    // this sound can be heard from.
    audeo::set_distance_range_max(sound, 100);

    // The sound listener is at position (0, 0, 0) by default, facing in
    // direction (0, 0, -1), which represents forward (pointing into the
    // screen). In this example, we will make the sound listener face the other
    // way and move him 50 units in that direction

    // The supplied direction vector will be normalized by this function if
    // needed
    audeo::set_listener_forward(0, 0, 1);
    audeo::set_listener_position(0, 0, 50);

    // You can query the listener position and forward values using
    // audeo::get_listener_forward() and audeo::get_listener_position()

    // Our main loop
    while (true) {
        SDL_Event event;
        // Process window events
        while (SDL_PollEvent(&event)) {
            // Handle quit event (close button)
            if (event.type == SDL_QUIT) {
                quit_sdl(window);
                return;
            }

            if (sdl_key_event(event, SDLK_LEFT)) {
                // When the 'left' key is pressed, move the sound to a position
                // 50 units from the left of the listener
                audeo::set_position(sound, 50, 0, 0);
            }
            if (sdl_key_event(event, SDLK_RIGHT)) {
                // And to the right
                audeo::set_position(sound, -50, 0, 0);
            }
        }
    }

    // Quit sdl at the end of our application
    quit_sdl(window);
}
