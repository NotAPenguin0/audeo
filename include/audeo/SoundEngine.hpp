#ifndef AUDEO_SOUND_ENGINE_HPP_
#define AUDEO_SOUND_ENGINE_HPP_

#include "Sound.hpp"
#include "SoundSource.hpp"
#include "exception.hpp"
#include "vec3.hpp"

#include <cstddef>
#include <unordered_map>
#include <utility>

namespace audeo {

enum class OutputChannelCount { Mono = 1, Stereo = 2 };
enum class AudioFormat {
    // Unsigned 8-bit samples
    U8,
    // Signed 8-bit samples
    S8,
    // Unsigned 16-bit samples, little endian byte order
    U16LSB,
    // Signed 16-bit samples, little endian byte order
    S16LSB,
    // Unsigned 16-bit samples, big endian byte order
    U16MSB,
    // Signed 16-bit samples, big endian byte order
    S16MSB,
    // Unsigned 16-bit samples, system byte order
    U16SYS,
    // Signed 16-bit samples, system byte order
    S16SYS,
    // The default format, determined by SDL_Mixer
    Default
};

// Pass this value to in a loop_count parameter to make it loop forever
static constexpr int loop_forever = -1;

// The main class for audeo. You should only ever need one instance of this
// class.
class SoundEngine {
public:
    // Used internally to store active sounds
    struct SoundData {
        // The source this sound is coming from
        SoundSource* source;
        // The channel this sound is playing on. -1 for music
        int channel;
        // The current position of the sound. Only used when the sound is an
        // effect
        vec3f position;
    };

    // Initialization data for the sound engine.
    struct InitInfo {
        // The audio frequency to use. This value defaults to 22050, as this is
        // a common frequency for games
        unsigned int frequency = 22050;
        // The amount of output channels.
        OutputChannelCount output_channels = OutputChannelCount::Stereo;
        // The size of a chunk, in bytes. Default is 4096 bytes
        unsigned int chunk_size = 4096;
        // The format the audio samples will be in
        AudioFormat format = AudioFormat::Default;
        // The amount of effect channels to allocate. This effectively controls
        // how many sound effects can be played at once. Defaults to 16
        unsigned int effect_channels = 16;
    };

    explicit SoundEngine(InitInfo const& info);

    ~SoundEngine();

    // Returns the name of the currently active audio driver
    std::string get_audio_driver_name() const;

    // These functions check/control the status of the engine

    // Return true if the engine is playing a music track
    bool is_playing_music() const;

    // Returns the amount of effect channels currently allocated. This
    // corresponds to the amount of audio samples that can be played at once
    unsigned int effect_channel_count() const;

    // Allocates extra effect channels to reach count channels. If this amount
    // of channels has already been allocated, this function has no effect
    void allocate_effect_channels(unsigned int count);

    // Play a sound source. loop_count is the amount of times we loop the sound.
    // fade_in_ms is the amount of ms to fade in. Leave this at 0 (the default
    // value) to play the sound without fading in.
    // Returns an instance of audeo::Sound, which is used as a handle to control
    // a currently playing sound. Note that you can play the same sound source
    // multiple times at once, as long as it's a sound effect and not music.
    Sound
    play_sound(SoundSource& source, int loop_count = 0, int fade_in_ms = 0);

    // Functions to query status of a playing sound

    // Checks if a sound is valid
    bool is_valid(Sound sound) const;

    // Returns the volume of a playing sound. This is a value between 0 and 1.
    // It is safe to call this function on paused sounds. If the sound is not
    // valid, this function will return -1
    float get_volume(Sound sound) const;

    // Returns the position of a playing sound. For music, this position will
    // always be (0, 0, 0). For an invalid sound, this function will also
    // return (0, 0, 0)
    vec3f get_position(Sound sound) const;

    // Functions to affect currently playing sounds. Note that all these
    // functions return a bool indicating success or failure.

    // Pauses a currently playing sound. Calling this on a paused sound has no
    // effect
    bool pause_sound(Sound sound);

    // Resumes a paused sound. Calling this on a sound that isn't paused has no
    // effect
    bool resume_sound(Sound sound);

    // Completely stops a sound. After this call, the sound handle will become
    // invalid, as the sound will no longer be playing
    bool stop_sound(Sound sound, int fade_out_ms = 0);

    // Set volume for a sound. This volume value is an value between 0 and 1,
    // where 0 means complete silence, and 1 means max volume. Any value outside
    // this range will be clamped to be inside it
    bool set_volume(Sound sound, float volume);

    // Set the 3D position of the sound
    bool set_position(Sound sound, vec3f position);
    bool set_position(Sound sound, float x, float y, float z);

    // Functionality to control positional audio.

    // Sets the audio listener position to specified position
    void set_listener_position(vec3f new_position);
    void set_listener_position(float new_x, float new_y, float new_z);

private:
    Sound play_music(SoundSource& source, int loop_count, int fade_in_ms);
    std::pair<Sound, int>
    play_effect(SoundSource& source, int loop_count, int fade_in_ms);

    void set_effect_position(int channel, vec3f position);

    std::unordered_map<Sound, SoundData> active_sounds;
    std::unordered_map<int, Sound> channel_map;

    // Default constructed to (0, 0, 0)
    vec3f listener_pos;
};

} // namespace audeo

#endif
