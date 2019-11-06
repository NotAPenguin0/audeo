#ifndef AUDEO_SOUND_ENGINE_HPP_
#define AUDEO_SOUND_ENGINE_HPP_

#include "Sound.hpp"
#include "SoundSource.hpp"
#include "exception.hpp"
#include "export_import.hpp"
#include "vec3.hpp"

#include <cstddef>
#include <functional>
#include <optional>
#include <string_view>
#include <utility>

namespace audeo {

namespace detail {
AUDEO_API inline void no_callback(Sound) {}
} // namespace detail

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

enum class Effect {
    // For the echo effect to be fully heard at the end of your sample, it is
    // recommended that you add some silence to the end of it so that it will
    // actually be heard properly. If you don't do this, the effect will be cut
    // off at the end
    Echo,
    None
};

struct loop_forever_t {};

// Pass this value to in a loop_count parameter to make it loop forever
static constexpr loop_forever_t loop_forever;

using SoundFinishCallbackT = std::function<void(Sound)>;

// Used internally to store active sounds
struct SoundData {
    // The source this sound is coming from
    SoundSource source;
    // The channel this sound is playing on. -1 for music
    int channel;
    // The current position of the sound. Only used when the sound is an
    // effect
    vec3f position;
    // The maximum distance this sound can be heard from. Only used when the
    // sound is an effect
    float max_distance;
};

// Initialization data for the sound engine.
struct InitInfo {
    // The audio frequency to use. This value defaults to 22050, as this is
    // a common frequency for games
    unsigned int frequency = 22050;
    // The amount of output channels.
    OutputChannelCount output_channels = OutputChannelCount::Stereo;
    // The size of a chunk, in bytes. Default is 4096 bytes
    unsigned int chunk_size = 8192;
    // The format the audio samples will be in
    AudioFormat format = AudioFormat::Default;
    // The amount of effect channels to allocate. This effectively controls
    // how many sound effects can be played at once. Defaults to 16
    unsigned int effect_channels = 16;
};

AUDEO_API bool init(InitInfo const& info = InitInfo {});
 
AUDEO_API void quit();

// Returns the name of the currently active audio driver
AUDEO_API std::string get_audio_driver_name();

// These functions check/control the status of the engine

// Return true if the engine is playing a music track
AUDEO_API bool is_playing_music();

// Returns the amount of effect channels currently allocated. This
// corresponds to the amount of audio samples that can be played at once
AUDEO_API unsigned int effect_channel_count();

// Allocates extra effect channels to reach count channels. If this amount
// of channels has already been allocated, this function has no effect
AUDEO_API void allocate_effect_channels(unsigned int count);

// Functions that control sound sources

// Loads a sound source into memory. This returns a handle that you can pass to
// the library to do stuff with your sound source
[[nodiscard]] AUDEO_API SoundSource load_source(std::string_view path,
                                               AudioType type);

// This will free a sound source if it is not currently playing. Returns the
// success of the function
AUDEO_API bool free_source(SoundSource source);

// Frees all sources that are not currently playing. Returns the amount of
// sources freed
AUDEO_API std::size_t free_unused_sources();

// Returns whether a sound source currently has a playing Sound instance
// attached to it
AUDEO_API bool is_playing(SoundSource source);

// Returns true if the sound was loaded as a music sound
AUDEO_API bool source_is_music(SoundSource source);

// The volume parameter is a value between 0 and 1, where 0 means silent and
// 1 means max volume. Any value outside this range will be clamped to fit
// the range
AUDEO_API bool set_default_volume(SoundSource source, float volume);

// Used for 3D spatial audio. This is a position in world space and will be
// used together with the listener position to create 3D sounds
AUDEO_API bool
set_default_position(SoundSource source, float x, float y, float z);
AUDEO_API bool set_default_position(SoundSource source, vec3f position);

// Set the maximum distance this sound can be heard from. This defaults to
// 255 units
AUDEO_API bool set_default_distance_range_max(SoundSource source,
                                              float distance);

// Functions that control sounds

// Play a sound source. loop_count is the amount of times we loop the sound.
// fade_in_ms is the amount of ms to fade in. Leave this at 0 (the default
// value) to play the sound without fading in.
// Returns an instance of audeo::Sound, which is used as a handle to control
// a currently playing sound. Note that you can play the same sound source
// multiple times at once, as long as it's a sound effect and not music.
AUDEO_API Sound play_sound(SoundSource source,
                           int loop_count = 0,
                           int fade_in_ms = 0);

AUDEO_API Sound play_sound(SoundSource source,
                           loop_forever_t,
                           int fade_in_ms = 0);

// Functions to query status of a playing sound

// Checks if a sound is valid
AUDEO_API bool is_valid(Sound sound);

// Checks if a sound source is valid
AUDEO_API bool is_valid(SoundSource source);

// Returns the volume of a playing sound. This is a value between 0 and 1.
// It is safe to call this function on paused sounds.
AUDEO_API std::optional<float> get_volume(Sound sound);

// Returns the position of a playing sound. For music, this position will
// always be (0, 0, 0). For an invalid sound, this function will also
// return (0, 0, 0)
AUDEO_API std::optional<vec3f> get_position(Sound sound);

// Returns the listener position. If no listener position was set, this will
// be (0, 0, 0)
AUDEO_API vec3f get_listener_position();

// Returns the forward direction vector of the listener. If no listener
// forward direction was set, this will default to be (0, 0, -1) (which is
// forward in OpenGL)
AUDEO_API vec3f get_listener_forward();

// Functions to affect currently playing sounds. Note that all these
// functions return a bool indicating success or failure.

// Pauses a currently playing sound. Calling this on a paused sound has no
// effect
AUDEO_API bool pause_sound(Sound sound);

// Resumes a paused sound. Calling this on a sound that isn't paused has no
// effect
AUDEO_API bool resume_sound(Sound sound);

// Completely stops a sound. After this call, the sound handle will become
// invalid, as the sound will no longer be playing
AUDEO_API bool stop_sound(Sound sound, int fade_out_ms = 0);

// Set volume for a sound. This volume value is an value between 0 and 1,
// where 0 means complete silence, and 1 means max volume. Any value outside
// this range will be clamped to be inside it
AUDEO_API bool set_volume(Sound sound, float volume);

// Set the 3D position of the sound
AUDEO_API bool set_position(Sound sound, vec3f position);
AUDEO_API bool set_position(Sound sound, float x, float y, float z);

// Set the maximum distance this sound can be heard from
AUDEO_API bool set_distance_range_max(Sound sound, float distance);

// Functionality to control the positional audio.

// Sets the audio listener position to specified position
AUDEO_API void set_listener_position(vec3f new_position);
AUDEO_API void set_listener_position(float new_x, float new_y, float new_z);

// Set the forward direction for the listener to the new forward direction
// vector
AUDEO_API void set_listener_forward(vec3f new_forward);
AUDEO_API void set_listener_forward(float new_x, float new_y, float new_z);

// Callbacks and special effects

// Swaps stereo left and right. This function only has effect when
// initialized with stereo audio. To reverse this effect, call this function
// with false as the second argument
AUDEO_API bool reverse_stereo(Sound sound, bool reverse = true);

AUDEO_API bool add_effect(Sound sound, Effect effect);

// Set a callback that is called right after the sound is stopped, and right
// before it is removed from the system. This means that the sound parameter
// is still valid inside the callback function
AUDEO_API void set_sound_finish_callback(SoundFinishCallbackT callback);

} // namespace audeo

#endif
