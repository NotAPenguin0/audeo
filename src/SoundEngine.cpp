#include "audeo/SoundEngine.hpp"
#include "audeo/effects.hpp"

// SDL headers
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_mixer.h>

#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>

namespace audeo {

namespace {

// Functions and data that control the engine's state

struct SoundSourceData {
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

    bool is_music = false;

    data_t data;
    DefaultParameters default_params;
};

std::unordered_map<SoundSource, SoundSourceData> sound_sources;
std::unordered_map<Sound, SoundData> active_sounds;
std::unordered_map<int, Sound> channel_map;

// Default constructed to (0, 0, 0)
vec3f listener_pos;
vec3f listener_forward = {0.0f, 0.0f, -1.0f};

SoundFinishCallbackT finish_callback = detail::no_callback;

template<typename T>
struct HandleGenerator {
    static std::int64_t cur;
    static std::int64_t next() { return cur++; }
};

template<typename T>
std::int64_t HandleGenerator<T>::cur = 0;

using SoundHandleGenerator = HandleGenerator<Sound>;
using SourceHandleGenerator = HandleGenerator<SoundSource>;

struct SoundFinishedCallbacks {
    static void remove_sound_from_map(int channel) {
        if (auto sound_it = channel_map.find(channel);
            sound_it != channel_map.end()) {
            Sound sound = sound_it->first;
            finish_callback(sound);
            channel_map.erase(sound_it);
            active_sounds.erase(sound);
        } else {
            AUDEO_THROW(audeo::exception("Invalid channel"));
        }
    }

    static void channel_callback(int channel) {
        remove_sound_from_map(channel);
        // Unregister all effects from this channel, so that they won't apply to
        // the next sound that plays here
        Mix_UnregisterAllEffects(channel);
    }
    static void music_callback() {
        // -1 is the music channel in the channel map
        remove_sound_from_map(-1);
    }
};

float map_range(float a, float b, float c, float d, float x) {
    // https://math.stackexchange.com/questions/377169/calculating-a-value-inside-one-range-to-a-value-of-another-range
    return (x - a) * ((d - c) / (b - a)) + c;
}

} // namespace

static Sound play_music(SoundSource source, int loop_count, int fade_in_ms);
static std::pair<Sound, int>
play_effect(SoundSource source, int loop_count, int fade_in_ms);

static void
set_effect_position(int channel, vec3f position, float max_distance);

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

bool init(InitInfo const& info) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::string error = SDL_GetError();
        AUDEO_THROW(audeo::exception(
            ("Audeo: Unable to initliaze SDL audeo. Reason: " + error)
                .c_str()));
        // This return can only be reached when exceptions are disabled
        return false;
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
        return false;
    }
    // Load dynamic libraries for SDL_Mixer
    const int flags =
        MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_MP3;
    if ((flags & Mix_Init(flags)) != flags) {
        std::string error = Mix_GetError();
        AUDEO_THROW(audeo::exception(
            ("Audeo: Could not load all audio types. Reason: " + error)
                .c_str()));
        return false;
    }
    // Allocate channels for effects
    Mix_AllocateChannels(info.effect_channels);

    // Initialize callbacks
    Mix_HookMusicFinished(&SoundFinishedCallbacks::music_callback);
    Mix_ChannelFinished(&SoundFinishedCallbacks::channel_callback);

    return true;
}

void quit() {
    Mix_HookMusicFinished(nullptr);
    Mix_ChannelFinished(nullptr);

    // Stop SDL and SDL_Mixer subsystems
    Mix_CloseAudio();
    SDL_Quit();
    Mix_Quit();
}

std::string get_audio_driver_name() { return SDL_GetCurrentAudioDriver(); }

bool is_playing_music() { return Mix_PlayingMusic(); }

unsigned int effect_channel_count() { return Mix_AllocateChannels(-1); }

void allocate_effect_channels(unsigned int count) {
    if (effect_channel_count() >= count) { return; }
    Mix_AllocateChannels(count);
}

[[nodiscard]] SoundSource load_source(std::string_view path, AudioType type) {
    SoundSource source(SourceHandleGenerator::next());
    SoundSourceData source_data;
    switch (type) {
        case AudioType::Music:
            source_data.is_music = true;
            source_data.data.music = Mix_LoadMUS(path.data());
            break;
        case AudioType::Effect:
            source_data.data.chunk = Mix_LoadWAV(path.data());
            break;
    }

    // Check for errors
    if (source_data.is_music) {
        if (!source_data.data.music) {
            AUDEO_THROW(audeo::exception("Audeo: Failed to load music file"));
        }
    } else {
        if (!source_data.data.chunk) {
            AUDEO_THROW(audeo::exception("Audeo: Failed to load audio chunk"));
        }
    }

    sound_sources[source] = source_data;

    return source;
}

bool free_source(SoundSource source) {
    if (!is_valid(source)) { return false; }
    if (is_playing(source)) { return false; }

    SoundSourceData& data = sound_sources[source];
    if (data.is_music) {
        Mix_FreeMusic(data.data.music);
    } else {
        Mix_FreeChunk(data.data.chunk);
    }

    // Remove from sound source map
    sound_sources.erase(source);

    return true;
}

std::size_t free_unused_sources() {
    // Create list of sources that have to be freed
    std::vector<SoundSource> to_erase;
    for (auto const& [source, data] : sound_sources) {
        if (!is_playing(source)) { to_erase.push_back(source); }
    }

    // Free all sources that have to be freed
    for (auto src : to_erase) { free_source(src); }

    return to_erase.size();
}

bool is_playing(SoundSource source) {
    if (!is_valid(source)) { return false; }

    for (auto const& [snd, data] : active_sounds) {
        if (data.source == source) { return true; }
    }
    return false;
}

bool source_is_music(SoundSource source) {
    if (!is_valid(source)) { return false; }

    SoundSourceData const& data = sound_sources[source];
    return data.is_music;
}

bool set_default_volume(SoundSource source, float volume) {
    if (!is_valid(source)) { return false; }

    // Clamp volume parameter
    if (volume > 1) volume = 1;
    if (volume < 0) volume = 0;

    SoundSourceData& data = sound_sources[source];
    data.default_params.volume = volume;

    return true;
}

bool set_default_position(SoundSource source, float x, float y, float z) {
    return set_default_position(source, {x, y, z});
}

bool set_default_position(SoundSource source, vec3f position) {
    if (!is_valid(source)) { return false; }

    SoundSourceData& data = sound_sources[source];
    data.default_params.position = position;

    return true;
}

bool set_default_distance_range_max(SoundSource source, float distance) {
    if (!is_valid(source)) { return false; }

    SoundSourceData& data = sound_sources[source];
    data.default_params.distance_range_max = distance;

    return true;
}

Sound play_sound(SoundSource source, int loop_count, int fade_in_ms /* = 0 */) {

    Sound sound(-1);

    if (!is_valid(source)) { return sound; }

    SoundData data;
    data.source = source;

    SoundSourceData const& source_data = sound_sources[source];

    if (source_is_music(source)) {
        sound = play_music(source, loop_count, fade_in_ms);
        data.channel = -1;
    } else {
        // play_effect returns a pair with the sound and the channel it is
        // played on
        auto effect_data = play_effect(source, loop_count, fade_in_ms);
        sound = effect_data.first;
        data.channel = effect_data.second;
        data.position = source_data.default_params.position;
    }
    // Add the sound to the active sounds list and to the channel map
    active_sounds.try_emplace(sound, data);
    channel_map[data.channel] = sound;

    return sound;
}

Sound play_sound(SoundSource source, loop_forever_t, int fade_in_ms /* = 0 */) {
    // Play the sound with the loop_forever parameter, which is -1
    return play_sound(source, -1, fade_in_ms);
}

bool is_valid(Sound sound) {
    return active_sounds.find(sound) != active_sounds.end();
}

bool is_valid(SoundSource source) {
    return sound_sources.find(source) != sound_sources.end();
}

std::optional<float> get_volume(Sound sound) {
    if (!is_valid(sound)) { return std::nullopt; }

    SoundData const& data = active_sounds.at(sound);

    if (source_is_music(data.source)) {
        return static_cast<float>(Mix_VolumeMusic(-1)) / MIX_MAX_VOLUME;
    } else {
        return static_cast<float>(Mix_Volume(data.channel, -1)) /
               MIX_MAX_VOLUME;
    }
}

std::optional<vec3f> get_position(Sound sound) {
    if (!is_valid(sound)) { return std::nullopt; }

    SoundData const& data = active_sounds.at(sound);

    // No need to check or music first, as positions for music are always (0, 0,
    // 0), which is the specified return value
    return data.position;
}

vec3f get_listener_position() { return listener_pos; }

vec3f get_listener_forward() { return listener_forward; }

bool pause_sound(Sound sound) {
    // Check if the sound is valid first
    if (!is_valid(sound)) { return false; }

    // Find the sound data
    SoundData const& data = active_sounds[sound];

    // Pause the music channel or the sound's channel, depending on what
    // this sound is
    if (source_is_music(data.source)) {
        Mix_PauseMusic();
    } else {
        Mix_Pause(data.channel);
    }

    // Mission success
    return true;
}

bool resume_sound(Sound sound) {
    if (!is_valid(sound)) { return false; }

    SoundData const& data = active_sounds[sound];

    if (source_is_music(data.source)) {
        Mix_ResumeMusic();
    } else {
        Mix_Resume(data.channel);
    }

    return true;
}

bool stop_sound(Sound sound, int fade_out_ms) {
    if (!is_valid(sound)) { return false; }

    SoundData const& data = active_sounds[sound];

    if (source_is_music(data.source)) {
        Mix_FadeOutMusic(fade_out_ms);
    } else {
        Mix_FadeOutChannel(data.channel, fade_out_ms);
    }

    return true;
}

bool set_volume(Sound sound, float volume) {
    if (!is_valid(sound)) { return false; }

    if (volume > 1) volume = 1;
    if (volume < 0) volume = 0;

    SoundData const& data = active_sounds[sound];

    if (source_is_music(data.source)) {
        Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * volume));
    } else {
        Mix_Volume(data.channel, static_cast<int>(MIX_MAX_VOLUME * volume));
    }

    return true;
}

bool set_position(Sound sound, float x, float y, float z) {
    return set_position(sound, {x, y, z});
}

bool set_position(Sound sound, vec3f position) {
    if (!is_valid(sound)) { return false; }

    SoundData& data = active_sounds[sound];

    // Music does not support 3D spatial audio
    if (source_is_music(data.source)) { return false; }
    // Set the actual position of the effect
    set_effect_position(data.channel, position, data.max_distance);
    data.position = position;

    return true;
}

bool set_distance_range_max(Sound sound, float distance) {
    if (!is_valid(sound)) { return false; }

    SoundData& data = active_sounds[sound];

    // Music does not support 3D spatial audio
    if (source_is_music(data.source)) { return false; }

    data.max_distance = distance;
    // Update positional sound data
    set_effect_position(data.channel, data.position, data.max_distance);

    return true;
}

void set_listener_position(vec3f new_position) {
    listener_pos = new_position;
    // Now, update all positions for playing sounds
    for (auto const& [snd, data] : active_sounds) {
        set_position(snd, data.position);
    }
}

void set_listener_position(float new_x, float new_y, float new_z) {
    set_listener_position({new_x, new_y, new_z});
}

void set_listener_forward(vec3f new_forward) {
    listener_forward = new_forward;
    // Now, update playing sound positions
    for (auto const& [snd, data] : active_sounds) {
        set_position(snd, data.position);
    }
}

void set_listener_forward(float new_x, float new_y, float new_z) {
    set_listener_forward({new_x, new_y, new_z});
}

bool reverse_stereo(Sound sound, bool reverse /* = true */) {
    if (!is_valid(sound)) { return false; }

    SoundData& data = active_sounds[sound];

    // If the second parameter is zero (false), the effect will unregister.
    Mix_SetReverseStereo(data.channel, reverse);

    return true;
}

bool add_effect(Sound sound, Effect eff) {
    if (!is_valid(sound)) { return false; }

    SoundData& data = active_sounds[sound];

    // Temporary always register echo
    Mix_RegisterEffect(data.channel, echo_callback, nullptr, nullptr);

    return true;
}

void set_sound_finish_callback(SoundFinishCallbackT callback) {
    finish_callback = std::move(callback);
}

// Internal functions

static Sound play_music(SoundSource source, int loop_count, int fade_in_ms) {

    SoundSourceData const& data = sound_sources[source];

    Mix_FadeInMusic(data.data.music, loop_count, fade_in_ms);
    Mix_VolumeMusic(
        static_cast<int>(MIX_MAX_VOLUME * data.default_params.volume));

    return Sound(SoundHandleGenerator::next());
}

static std::pair<Sound, int>
play_effect(SoundSource source, int loop_count, int fade_in_ms) {

    SoundSourceData const& data = sound_sources[source];
    auto const& default_params = data.default_params;

    int channel =
        Mix_FadeInChannel(-1, data.data.chunk, loop_count, fade_in_ms);

    if (channel == -1) {
        AUDEO_THROW(
            audeo::exception("No channel available to play sound effect"));
    }

    // Set volume
    Mix_Volume(channel,
               static_cast<int>(MIX_MAX_VOLUME * default_params.volume));

    set_effect_position(channel, default_params.position,
                        default_params.distance_range_max);

    return {Sound(SoundHandleGenerator::next()), channel};
}

static void
set_effect_position(int channel, vec3f position, float max_distance) {
    vec3f direction = position - listener_pos;
    vec3f forward = normalize(listener_forward);
    float raw_angle = angle(forward, direction);
    // Now get the distance
    float raw_distance = magnitude(direction);
    // Now we adjust the angle to the left, depending on whether it is to
    // the left or to the right of the listener. We test this by testing the
    // sign of the cross product.
    float d = dot(cross(direction, forward), vec3f{0, 1, 0});

    if (d < 0) { raw_angle += 180.0f; }

    // Maximum distance SDL provides
    constexpr std::uint8_t sdl_max_distance = 255;
    // Map our own max distance to SDL's max_distance

    std::uint8_t mapped_distance = static_cast<std::int8_t>(
        map_range(0, max_distance, 0, sdl_max_distance, raw_distance));

    Mix_SetPosition(channel, static_cast<std::int16_t>(raw_angle),
                    mapped_distance);
}

} // namespace audeo
