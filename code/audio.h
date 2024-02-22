#pragma once

#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>

using namespace std;

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

    HWAVEOUT hWaveOut; // Plaform-specific handle to the audio device
    WAVEFORMATEX waveFormat;
    WAVEHDR waveHeader;

    static const int bufferSamples = 8192;
    static const int bufferSize = bufferSamples * 2 * 16 / 8; // 64 samples, 2 channels, 16 bits per sample, 8 bits per byte
    BYTE buffer[bufferSize]; 
    int buffersPlayed = 0; 
    Sound sound;

    Audio() {
        Sound sound;
        combine_waves();

        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2;
        waveFormat.nSamplesPerSec = 44100;
        waveFormat.wBitsPerSample = 16;
        waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

        MMRESULT result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, reinterpret_cast<DWORD_PTR>(waveOutProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
        if (result != MMSYSERR_NOERROR) {
            std::cerr << "Error opening audio device" << std::endl;
        }
        waveOutProc(hWaveOut, WOM_DONE, reinterpret_cast<DWORD_PTR>(this), 0, 0);
        
    }

    ~Audio() {
        waveOutClose(hWaveOut);
    }

    void send_buffer_to_audio_device() {
        waveHeader.lpData = reinterpret_cast<LPSTR>(buffer);
        waveHeader.dwBufferLength = bufferSize;
        waveHeader.dwFlags = 0;
        waveHeader.dwLoops = 0;
        waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));
    }

    void combine_waves() {

            for (int i = 0; i < bufferSize; ++i) {
                buffer[i] = sound.wave[(i + (buffersPlayed * bufferSize)) % sound.wave.size()];
            }
        }

    static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        if (uMsg != WOM_DONE) { return; }


        Audio* audio = reinterpret_cast<Audio*>(dwInstance);
        audio->send_buffer_to_audio_device();
        audio->combine_waves();
        audio->buffersPlayed++;
    
    }
};