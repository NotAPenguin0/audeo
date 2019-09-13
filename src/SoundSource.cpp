#include "audeo/SoundSource.hpp"

namespace audeo {

SoundSource::SoundSource(std::string_view path, AudioType type) {
    load(path, type);
}

SoundSource::SoundSource(SoundSource&& rhs) {
    data_is_music = rhs.data_is_music;
    // Both are of the same type so it doesn't matter which one we set
    data.music = rhs.data.music;
    rhs.data.music = nullptr;
}

SoundSource::~SoundSource() { free_if_not_null(); }

SoundSource& SoundSource::operator=(SoundSource&& rhs) {
    if (this != &rhs) {
        data_is_music = rhs.data_is_music;
        // Both are of the same type so it doesn't matter which one we set
        data.music = rhs.data.music;
        rhs.data.music = nullptr;
    }
    return *this;
}

void SoundSource::load(std::string_view path, AudioType type) {
    free_if_not_null();
    switch (type) {
        case AudioType::Music:
            data_is_music = true;
            data.music = Mix_LoadMUS(path.data());
            break;
        case AudioType::Effect: data.chunk = Mix_LoadWAV(path.data()); break;
    }

    // Check for errors
    if (data_is_music) {
        if (!data.music) {
            AUDEO_THROW(audeo::exception("Audeo: Failed to load music file"));
        }
    } else {
        if (!data.chunk) {
            AUDEO_THROW(audeo::exception("Audeo: Failed to load audio chunk"));
        }
    }
}

bool SoundSource::is_music() const { return data_is_music; }

void SoundSource::set_default_volume(float volume) {
    if (volume > 1) volume = 1;
    if (volume < 0) volume = 0;

    default_params.volume = volume;
}

void SoundSource::set_default_position(vec3f position) {
    default_params.position = position;
}

void SoundSource::set_default_position(float x, float y, float z) {
    default_params.position = {x, y, z};
}

void SoundSource::free_if_not_null() {
    // Since both union elements are pointers it does not matter which one we
    // check, as both will be null when one is null anyway
    if (data.music) {
        if (data_is_music) {
            Mix_FreeMusic(data.music);
        } else {
            Mix_FreeChunk(data.chunk);
        }
    }
}

} // namespace audeo
