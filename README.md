# Audeo

Audeo is a library for playing 3D sounds for modern C++. Built on top of SDL_Mixer, I created this library because I couldn't find any satisfactory, lightweight wrappers around OpenAL. 

The purpose of audeo is to make playing and controlling 3D (but also 2D) sounds easy.

## Example 

```cpp
#include <audeo/audeo.hpp>

int main() {
    
    if (!audeo::init()) {
        return -1;
    }
    
    audeo::SoundSource music_source = audeo::load_source(
        "test_samples/happy_music.mp3", audeo::AudioType::Music);

    audeo::SoundSource effect_source = audeo::load_source(
        "test_samples/bell.wav", audeo::AudioType::Effect);

    audeo::Sound music = audeo::play_sound(music_source, 1);
    audeo::Sound effect = audeo::play_sound(effect_source);

    while (audeo::is_playing_music()) {}
    
    audeo::quit();
    
    return 0;
}
```

More examples can be found in the `examples` directory.

## You made a typo in the name! It's audio, not audeo!

No, I didn't. `audeo` is latin. See also https://latin-dictionary.net/definition/5532/audeo-audere-ausus

## Building

Audeo comes with a `CMakeLists.txt` file. All you have to do is make sure you have the SDL and SDL_Mixer libraries on your system. For Windows x64, these are already provided and no extra steps are needed.

## Questions

For any questions about the library or how to set it up, you're always welcome to open an issue, or shoot me a message on discord: `NotAPenguin#8928`, and I'll try to respond as soon as I can.

## Contributing

Any contributions, PR's, issues or questions are always welcome.
