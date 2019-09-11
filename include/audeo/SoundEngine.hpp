#ifndef AUDEO_SOUND_ENGINE_HPP_
#define AUDEO_SOUND_ENGINE_HPP_

#include "SoundSource.hpp"
#include "exception.hpp"

#include <cstddef>

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
    };

    explicit SoundEngine(InitInfo const& info);

    ~SoundEngine();

    // Returns the name of the currently active audio driver
    std::string get_audio_driver_name() const;

    // These functions check the status of the engine

    // Return true if the engine is playing a music track
    bool is_playing_music() const;

    // Play a music source. loop_count is the amount of times we loop the music.
    // fade_in_ms is the amount of ms to fade in. Leave this at 0 (the default
    // value) to play the music without fading in.
    void play_music(SoundSource& source, int loop_count, int fade_in_ms = 0);
};

} // namespace audeo

#endif
