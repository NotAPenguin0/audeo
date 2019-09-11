#include "audeo/SoundEngine.hpp"

// SDL headers
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_mixer.h>

#include <string>

namespace audeo {

static int to_mix_format(AudioFormat format) {
    switch (format) {
        case AudioFormat::U8: return AUDIO_U8;
        case AudioFormat::S8: return AUDIO_S8;
        case AudioFormat::U16LSB: return AUDIO_U16LSB;
        case AudioFormat::S16LSB: return AUDIO_S16LSB;
        case AudioFormat::U16MSB: return AUDIO_U16MSB;
        case AudioFormat::S16MSB: return AUDIO_S16MSB;
        case AudioFormat::U16SYS: return AUDIO_U16SYS;
        case AudioFormat::S16SYS: return AUDIO_S16SYS;
        case AudioFormat::Default: return MIX_DEFAULT_FORMAT;
    }
    return MIX_DEFAULT_FORMAT;
}

SoundEngine::SoundEngine(InitInfo const& info) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::string error = SDL_GetError();
        AUDEO_THROW(audeo::exception(
            ("Audeo: Unable to initliaze SDL. Reason: " + error).c_str()));
    }
    // Initialize SDL_Mixer
    if (Mix_OpenAudio(info.frequency, to_mix_format(info.format),
                      static_cast<int>(info.output_channels),
                      info.chunk_size) == -1) {
        // Mix_GetError() is the same as SDL_GetError()
        std::string error = Mix_GetError();
        AUDEO_THROW(audeo::exception(
            ("Audeo: Unable to initialize SDL_Mixer. Reason: " + error)
                .c_str()));
    }
    // Load dynamic libraries for SDL_Mixer
    const int flags =
        MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_MP3;
    if ((flags & Mix_Init(flags)) != flags) {
        std::string error = Mix_GetError();
        AUDEO_THROW(audeo::exception(
            ("Audeo: Could not load all audio types. Reason: " + error)
                .c_str()));
    }
}

SoundEngine::~SoundEngine() {
    // Stop SDL and SDL_Mixer subsystems
    Mix_CloseAudio();
    SDL_Quit();
    Mix_Quit();
}

std::string SoundEngine::get_audio_driver_name() const {
    return SDL_GetCurrentAudioDriver();
}

bool SoundEngine::is_playing_music() const { return Mix_PlayingMusic(); }

void SoundEngine::play_sound(SoundSource& source,
                             int loop_count,
                             int fade_in_ms /* = 0 */) {

    if (source.is_music()) {
        play_music(source, loop_count, fade_in_ms);
    } else {
		// play_effect(source, loop_count, fade_in_ms);
	}
}

void SoundEngine::play_music(SoundSource& source,
                             int loop_count,
                             int fade_in_ms) {
    if (fade_in_ms) {
        Mix_FadeInMusic(source.data.music, loop_count, fade_in_ms);
    } else {
        Mix_PlayMusic(source.data.music, loop_count);
    }
}

} // namespace audeo
