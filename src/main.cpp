#include "audeo/audeo.hpp"
#include <iostream>
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL.h>

// #TODO: Allow setting max distance (maybe per sound). The mixer will then map
// this to the SDL_Mixer sound range (0-255)

// #TODO: Special effects + user defined effects

// #TODO: Sound event callbacks

#define _USE_MATH_DEFINES
#include <cmath>

audeo::vec3f rotate_vec(audeo::vec3f vec, float angle) {
    audeo::vec3f result;
    // We're doing 2D at the moment
    result.y = 0;
    result.x = cos(angle) * vec.x - sin(angle) * vec.z;
    result.z = sin(angle) * vec.x + cos(angle) * vec.z;

    return result;
}

int main() {
    try {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        auto window = SDL_CreateWindow("audeo testing", SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED, 100, 100,
                                       SDL_WINDOW_INPUT_FOCUS);

        audeo::SoundEngine::InitInfo info;
        info.output_channels = audeo::OutputChannelCount::Stereo;
        info.effect_channels = 32;
        audeo::SoundEngine engine(info);

        audeo::SoundSource music("test_samples/happy_music.mp3",
                                 audeo::AudioType::Music);
        music.set_default_volume(0.3f);
        audeo::SoundSource bell_source("test_samples/happy_music.mp3",
                                       audeo::AudioType::Effect);
        audeo::Sound sound = engine.play_sound(music, audeo::loop_forever);
        bell_source.set_default_position(0, 0, -180);
        audeo::Sound moving_bell;

        auto* keys = SDL_GetKeyboardState(nullptr);

        engine.set_listener_forward(0, 0, -1);

        while (true) {
            SDL_PumpEvents();
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p) {
                    engine.pause_sound(sound);
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) {
                    engine.resume_sound(sound);
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_b) {
                    moving_bell =
                        engine.play_sound(bell_source, audeo::loop_forever);
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_s) {
                    engine.stop_sound(sound);
                }
				if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                    static bool reverse = false;
                    engine.reverse_stereo(moving_bell, !reverse);
                    reverse = !reverse;
				}
            }
            constexpr float angle = 0.01f;
            if (keys[SDL_SCANCODE_LEFT]) {
                // rotate listener forward to the left
                audeo::vec3f fwd = engine.get_listener_forward();
                fwd = rotate_vec(fwd, angle);
                engine.set_listener_forward(fwd);
            }
            if (keys[SDL_SCANCODE_RIGHT]) {
                // same to the right
                audeo::vec3f fwd = engine.get_listener_forward();
                fwd = rotate_vec(fwd, -angle);
                engine.set_listener_forward(fwd);
            }
        }

        SDL_DestroyWindow(window);
        SDL_Quit();
    } catch (audeo::exception const& e) { std::cout << e.what(); }

    std::cin.get();

    return 0;
}
