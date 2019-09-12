#include "audeo/audeo.hpp"
#include <iostream>
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL.h>

int main() {
    try {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        auto window = SDL_CreateWindow("audeo testing", SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED, 100, 100,
                                       SDL_WINDOW_INPUT_FOCUS);

        audeo::SoundEngine::InitInfo info;
        info.output_channels = audeo::OutputChannelCount::Stereo;
        audeo::SoundEngine engine(info);

        audeo::SoundSource music("test_samples/happy_music.mp3",
                                 audeo::AudioType::Music);
        music.set_default_volume(0.3f);
        audeo::SoundSource bell_source("test_samples/bell.wav",
                                       audeo::AudioType::Effect);
        audeo::Sound sound = engine.play_sound(music, audeo::loop_forever);

        while (engine.is_playing_music()) {
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
                    engine.play_sound(bell_source);
				}
				if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
                    engine.set_volume(sound, engine.get_volume(sound) + 0.1f);
				}
				if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
                    engine.set_volume(sound, engine.get_volume(sound) - 0.1f);
				}
            }
        }

        SDL_DestroyWindow(window);
        SDL_Quit();
    } catch (audeo::exception const& e) { std::cout << e.what(); }

    return 0;
}
