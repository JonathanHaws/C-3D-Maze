#pragma once

#include <Windows.h>
#include <mmsystem.h>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>

struct Sound {

    const char* path;
    std::vector<BYTE> wave;

    void printData() {
        for (int i = 0; i < wave.size(); i++) { printf("%d ", wave[i]); }
    }

    void generate_sine_wave(int seconds, int sampleRate, int frequency) {
        wave.clear(); // Clear any existing wave data
        const double amplitude = 500.0; // Amplitude of the sine wave (maximum value for 16-bit audio)
        const double twoPi = 2.0 * 3.14159265358979323846;
        int numSamples = seconds * sampleRate; // Calculate the number of samples
        for (int i = 0; i < numSamples; ++i) {
            double time = static_cast<double>(i) / sampleRate;
            double value = amplitude * sin(twoPi * frequency * time);
            short sample = static_cast<short>(value);
            wave.push_back(static_cast<BYTE>(sample & 0xFF)); // Lower byte
            wave.push_back(static_cast<BYTE>((sample >> 8) & 0xFF)); // Upper byte
        }
    }

    Sound() {
        generate_sine_wave(1, 44100, 440);
    }

    void load_wave_file(const char* path) {
        std::ifstream file(path, std::ios::binary);
        if(file) {
            wave.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
            printf("Successfully read audio file: %s\n", path);
        } else {
            printf("Error opening audio file: %s\n", path);
        }
    }

};

struct Audio {
    HWAVEOUT hWaveOut;
    WAVEFORMATEX waveFormat;
    WAVEHDR waveHeader;

    int bufferSamples = 8192;
    const int bufferSize = bufferSamples * 2 * 16 / 8; // 64 samples, 2 channels, 16 bits per sample, 8 bits per byte
    int buffersPlayed = 0; 
    BYTE* buffer; 
    Sound sound;
    
    ~Audio() {
        delete[] buffer;
        waveOutClose(hWaveOut);
    }

    Audio() {
        buffer = new BYTE[bufferSize];
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2;
        waveFormat.nSamplesPerSec = 44100;
        waveFormat.wBitsPerSample = 16;
        waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
        waveHeader.lpData = reinterpret_cast<LPSTR>(buffer);
        waveHeader.dwBufferLength = bufferSize;
        waveHeader.dwFlags = 0;
        waveHeader.dwLoops = 0;
        MMRESULT result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, reinterpret_cast<DWORD_PTR>(waveOutProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
        if (result != MMSYSERR_NOERROR) {
            std::cerr << "Error opening audio device" << std::endl;
        }
        waveOutProc(hWaveOut, WOM_DONE, reinterpret_cast<DWORD_PTR>(this), 0, 0);
        Sound sound;
        
    }

    static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        if (uMsg == WOM_DONE) {
            Audio* audio = reinterpret_cast<Audio*>(dwInstance);
            audio->buffersPlayed++;

            // Calculate the index from which to start copying the sound data
            int startIndex = (audio->buffersPlayed - 1) * audio->bufferSize;

            // Calculate the remaining samples to be played
            int remainingSamples = audio->sound.wave.size() - startIndex;

            // Copy sound data into the buffer starting from the calculated index
            int copySize = (audio->bufferSize < remainingSamples) ? audio->bufferSize : remainingSamples;
            std::copy(audio->sound.wave.begin() + startIndex,
                    audio->sound.wave.begin() + startIndex + copySize,
                    audio->buffer);

            // If there are remaining samples, prepare and write the buffer
            if (remainingSamples > 0) {
                waveOutPrepareHeader(hwo, &(audio->waveHeader), sizeof(WAVEHDR));
                waveOutWrite(hwo, &(audio->waveHeader), sizeof(WAVEHDR));
            } else {
                // If all samples are played, stop playback or handle as needed
            }
        }
    }

};