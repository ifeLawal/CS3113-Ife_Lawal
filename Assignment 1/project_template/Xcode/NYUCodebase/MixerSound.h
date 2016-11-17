//
//  MixerSound.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 11/17/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef MixerSound_hpp
#define MixerSound_hpp

#include <SDL.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <vector>
#include <stdio.h>

class MixerSound {
public:
    Uint32 offset;
    Uint32 length;
    Uint8 *buffer;
    float volume;
    SDL_AudioFormat format;
    bool loaded;
    bool playing;
    bool loop;
};

class SoundGroups {
public:
    
    std::vector<MixerSound> mixSounds;
    
    int loadSound(const char *soundFile);
    
};

#endif /* MixerSound_hpp */
