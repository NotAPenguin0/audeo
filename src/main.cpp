#include "audeo/audeo.hpp"
#include <iostream>
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL.h>

// #TODO: Special effects + user defined effects

// #TODO: Example for special effects

// #TODO: Multithread everything? (use a queue to push events onto)

// #TODO: Set fade out when playing the music

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

int test_main() {
    try {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        auto window = SDL_CreateWindow("audeo testing", SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED, 100, 100,
                                       SDL_WINDOW_INPUT_FOCUS);

        audeo::InitInfo info;
        info.output_channels = audeo::OutputChannelCount::Stereo;
        info.effect_channels = 32;
        audeo::init(info);

        audeo::SoundSource music("test_samples/happy_music.mp3",
                                 audeo::AudioType::Music);
        music.set_default_volume(0.3f);
        audeo::SoundSource bell_source("test_samples/bell.wav",
                                       audeo::AudioType::Effect);
        audeo::Sound sound = audeo::play_sound(music, audeo::loop_forever);
        bell_source.set_default_position(0, 0, 9.5f);
        bell_source.set_default_distance_range_max(10.0f);
        audeo::Sound moving_bell;

        auto* keys = SDL_GetKeyboardState(nullptr);

        audeo::set_listener_forward(0, 0, 1);

        audeo::set_sound_finish_callback([](audeo::Sound snd) {
            std::cout << "Finished playing sound with ID " << snd.value()
                      << "\n";
        });

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
                    audeo::pause_sound(sound);
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) {
                    audeo::resume_sound(sound);
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_b) {
                    moving_bell = audeo::play_sound(bell_source);
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_s) {
                    audeo::stop_sound(sound);
                }
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                    static bool reverse = false;
                    audeo::reverse_stereo(moving_bell, !reverse);
                    reverse = !reverse;
                }
            }
            constexpr float angle = 0.01f;
            if (keys[SDL_SCANCODE_LEFT]) {
                // rotate listener forward to the left
                audeo::vec3f fwd = audeo::get_listener_forward();
                fwd = rotate_vec(fwd, angle);
                audeo::set_listener_forward(fwd);
            }
            if (keys[SDL_SCANCODE_RIGHT]) {
                // same to the right
                audeo::vec3f fwd = audeo::get_listener_forward();
                fwd = rotate_vec(fwd, -angle);
                audeo::set_listener_forward(fwd);
            }
        }

        SDL_DestroyWindow(window);
        SDL_Quit();
    } catch (audeo::exception const& e) { std::cout << e.what(); }

    std::cin.get();

    return 0;
}
