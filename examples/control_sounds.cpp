#include "audeo/audeo.hpp"

// In this example we will take a look at how to control sounds during
// playback (pause/resume, stop sounds, adjust volume)

// For this, we will create a small SDL app to be able to receive user
// input. This is obviously not required for audeo to work, but it provides
// a good way of showing how it works

#include <SDL.h>

SDL_Window* init_sdl() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    return SDL_CreateWindow("Audeo example app: Controlling sounds",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 200,
                            200, SDL_WINDOW_INPUT_FOCUS);
}

void quit_sdl(SDL_Window* window) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Utility function to check for sdl key events
bool sdl_key_event(SDL_Event const& event, int key) {
    return event.type == SDL_KEYDOWN && event.key.keysym.sym == key;
}

void control_sounds() {

    SDL_Window* window = init_sdl();

    // Load our music source
    audeo::SoundSource source("test_samples/happy_music.mp3",
                              audeo::AudioType::Music);

    // Start playing our music. Since we will be controlling it in this
    // application, we will have to store the sound handle that is returned by
    // audeo::play_sound(). Note that we pass audeo::loop_forever as the second
    // parameter here to loop the source until it is stopped by a call to
    // audeo::stop_sound()
    audeo::Sound music = audeo::play_sound(source, audeo::loop_forever);

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
            // If the 'p' key is pressed, pause the music
            if (sdl_key_event(event, SDLK_p)) {
                // Pausing a playing sound is as simple as giving a call to
                // audeo::pause_sound(). Note that it is safe to call this
                // function on already paused audio. The return value of this
                // function indicates the success of the function. This function
                // can fail if the sound passed in is no longer playing, as with
                // most playback control functions
                audeo::pause_sound(music);
            }
            // Pressing 'r' will resume the sound
            if (sdl_key_event(event, SDLK_r)) {
                // This function is also safe to call with an invalidated sound
                // handle, and behaves similar to pause_sound()
                audeo::resume_sound(music);
            }
            // Pressing 's' will completely stop the sound
            if (sdl_key_event(event, SDLK_s)) {
                // This function has an additional fade_out_ms parameter, which
                // can be used to control how long it takes for the sound to
                // fade out and stop playing
                audeo::stop_sound(music);
            }
            // Pressing the up arrow will set the sound volume to a high level,
            // pressing the down arrow will set it to a low level
            if (sdl_key_event(event, SDLK_UP)) {
                // The volume must be in range [0, 1]. If it is not in this
                // range, it will be clamped to fit
                audeo::set_volume(music, 1.0f);
            }
            if (sdl_key_event(event, SDLK_DOWN)) {
                // Set the music volume to a rather silent volume
                audeo::set_volume(music, 0.5f);
            }
        }
    }

    // Quit sdl at the end of our application
    quit_sdl(window);
}
