#include "audeo/audeo.hpp"

void basic_playback() {
    // Note that these examples rely on the main function in examples_main.cpp
    // to initialize the audeo library

    // Load two sound sources into memory. One for background music, and one for
    // an effect. Music will be streamed from the file, while an effect is fully
    // loaded into memory
    //
    // audeo::load_source() returns a handle (audeo::SoundSource) to an
    // audeo sound source that is loaded. You can free this source by calling
    // audeo::free_source(), or audeo::free_unused_sources(), which will free
    // all sound sources that are not playing

    audeo::SoundSource music_source = audeo::load_source(
        "test_samples/happy_music.mp3", audeo::AudioType::Music);

    audeo::SoundSource effect_source = audeo::load_source(
        "test_samples/bell.wav", audeo::AudioType::Effect);

    // Now we start the music by calling audeo::play_sound(). We will loop the
    // music once (the second parameter). This function will return an instance
    // of audeo::Sound you should use to refer to this currently playing sound.
    // Note that this means you can play the same sound source multiple times at
    // once (this does not work for music sources, as there is only a single
    // music channel provided). If you don't need to control the sound during
    // playback, it is safe to ignore this return value.
    audeo::Sound music = audeo::play_sound(music_source, 1);

    // We can also play the effect source the same way. The loop_count parameter
    // defaults to 0, which means the effect will only be played once
    audeo::Sound effect = audeo::play_sound(music_source);

    // Idle loop so that we don't quit the application before the music stops.
    while (audeo::is_playing_music()) {}
}
