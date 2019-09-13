#include "audeo/audeo.hpp"
#include <iostream>
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL.h>

// #TODO: Allow setting max distance (maybe per sound). The mixer will then map
// this to the SDL_Mixer sound range (0-255)

// #TODO: Special effects + user defined effects

// #TODO: Sound event callbacks

// #TODO: Reverse stereo enable/disable

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

        bell_source.set_default_position(250, 0, 0);

        audeo::Sound moving_bell;

        auto* keys = SDL_GetKeyboardState(nullptr);

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
            }
            if (keys[SDL_SCANCODE_LEFT]) {
                audeo::vec3f pos = engine.get_position(moving_bell);
                pos.x -= 0.001f;
                engine.set_position(moving_bell, pos);
            }
            if (keys[SDL_SCANCODE_RIGHT]) {
                audeo::vec3f pos = engine.get_position(moving_bell);
                pos.x += 0.001f;
                engine.set_position(moving_bell, pos);
            }
            if (keys[SDL_SCANCODE_UP]) {
                audeo::vec3f pos = engine.get_position(moving_bell);
                pos.z -= 0.001f;
                engine.set_position(moving_bell, pos);
            }
            if (keys[SDL_SCANCODE_DOWN]) {
                audeo::vec3f pos = engine.get_position(moving_bell);
                pos.z += 0.001f;
                engine.set_position(moving_bell, pos);
            }
        }

        SDL_DestroyWindow(window);
        SDL_Quit();
    } catch (audeo::exception const& e) { std::cout << e.what(); }

    std::cin.get();

    return 0;
}
