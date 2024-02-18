#pragma once

#include <vector>
#include <Windows.h>
#include <mmsystem.h>

struct Audio {
    
    std::vector<HWAVEOUT> soundHandles;

    ~Audio() {
        for (HWAVEOUT handle : soundHandles) {
            waveOutReset(handle);
            }
        }

    void playSound(const char* soundFilePath) {
        HWAVEOUT soundHandle = NULL;
        if (PlaySoundA(soundFilePath, NULL, SND_FILENAME | SND_ASYNC)) {
            soundHandles.push_back(soundHandle);
            }
        }
    };