#ifndef AUDEO_SOUND_SOURCE_HPP_
#define AUDEO_SOUND_SOURCE_HPP_

#include <SDL_mixer.h>
#include <string_view>

#include "exception.hpp"

namespace audeo {

enum class AudioType { Music, Chunk };

class SoundSource {
public:
    friend class SoundEngine;

    SoundSource() = default;
    explicit SoundSource(std::string_view path, AudioType type);
    explicit SoundSource(SoundSource&& rhs);

    ~SoundSource();

    SoundSource& operator=(SoundSource&& rhs);

    void load(std::string_view path, AudioType type);

    bool is_music() const;

private:
    void free_if_not_null();

    bool data_is_music = false;
    union {
        Mix_Chunk* chunk = nullptr;
        Mix_Music* music;
    } data;
};

} // namespace audeo

#endif
