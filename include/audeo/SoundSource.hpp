#ifndef AUDEO_SOUND_SOURCE_HPP_
#define AUDEO_SOUND_SOURCE_HPP_

#include <SDL_mixer.h>
#include <string_view>

#include "exception.hpp"
#include "vec3.hpp"

namespace audeo {

enum class AudioType { Music, Effect };

class SoundSource {
public:
    using data_t = union {
        Mix_Chunk* chunk = nullptr;
        Mix_Music* music;
    };

    struct DefaultParameters {
        // volume is a value between 0 and 1, where 0 means silent and 1 means
        // max volume
        float volume = 1.0f;
        // Default constructed to (0, 0, 0)
        vec3f position;
        // Maximum distance for this sound to be heard
        float distance_range_max = 255;
    };

    SoundSource() = default;
    explicit SoundSource(std::string_view path, AudioType type);
    explicit SoundSource(SoundSource&& rhs);

    ~SoundSource();

    SoundSource& operator=(SoundSource&& rhs);

    // This will delete any previously loaded sound first. Do not call this
    // while there is a Sound currently playing this SoundSource
    void load(std::string_view path, AudioType type);

    // Returns true if the sound was loaded as a music sound
    bool is_music() const;

    // Set default values for this sound source

    // The volume parameter is a value between 0 and 1, where 0 means silent and
    // 1 means max volume. Any value outside this range will be clamped to fit
    // the range
    void set_default_volume(float volume);

    // Used for 3D spatial audio. This is a position in world space and will be
    // used together with the listener position to create 3D sounds
    void set_default_position(vec3f position);
    void set_default_position(float x, float y, float z);

    // Set the maximum distance this sound can be heard from. This defaults to
    // 255 units
    void set_default_distance_range_max(float distance);

    // Used internally by the engine. You shouldn't need to call these functions
    data_t& get_data();
    DefaultParameters const& get_default_params() const;

private:
    void free_if_not_null();

    bool data_is_music = false;

    data_t data;

    DefaultParameters default_params;
};

} // namespace audeo

#endif
