//
//  MixerSound.cpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 11/17/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "MixerSound.h"
#include <SDL2_mixer/SDL_mixer.h>

int SoundGroups::loadSound(const char *soundFile) {
    Uint8 *buffer;
    SDL_AudioSpec spec;
    Uint32 bufferSize;
    
    /*
    if(SDL_LoadWave(soundFile, &spec, &buffer, &bufferSize) == NULL) {
        return -1;
    }
    
    SDL_AudioCVT cvt;
    SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq, deviceSpec.format, deviceSpec.channels, deviceSpec.freq);
    */
    
    return 1;
}

