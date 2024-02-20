#pragma once

#include <Windows.h>
#include <mmsystem.h>
#include <vector>
#include <fstream>
#include <cstdio>

struct Sound {
    const char* path;
    std::vector<BYTE> wave;

     Sound(const char* path) : path(path) {
        std::ifstream file(path, std::ios::binary);
        if (file) {
            wave.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
            printf("Successfully read audio file: %s\n", path);
        } else {
            printf("Error opening audio file: %s\n", path);
        }
        //printData();
    }

    void printData() {
        for (int i = 0; i < wave.size(); i++) {
            printf("%d ", wave[i]);
        }
    }
};


struct Audio {
    HWAVEOUT hWaveOut;
    WAVEHDR waveHeader;
    const int durationInSeconds = 2; // Duration of the sine wave in seconds
    const int sampleRate = 44100; // Sample rate (samples per second)
    const int numChannels = 2; // Number of channels
    const int bitsPerSample = 16; // Bits per sample
    const int bufferSize = sampleRate * numChannels * bitsPerSample / 8 * durationInSeconds; // Total size of the buffer
    BYTE* buffer; // Buffer to store the audio data

    Audio() {
        buffer = new BYTE[bufferSize];
        generateSineWave();
        initializeWaveHeader();
        initializeWaveOut();
    }

    ~Audio() {
        delete[] buffer;
        waveOutClose(hWaveOut);
    }

    void generateSineWave() {
        const double frequency = 440.0; // Frequency of the sine wave in Hz (A4 note)
        const double amplitude = 32767.0; // Amplitude of the sine wave (maximum value for 16-bit audio)
        const double twoPi = 2.0 * 3.14159265358979323846;

        for (int i = 0; i < bufferSize / 2; ++i) {
            double time = static_cast<double>(i) / sampleRate;
            double value = amplitude * sin(twoPi * frequency * time);
            short sample = static_cast<short>(value);
            buffer[2 * i] = sample & 0xFF; // Lower byte
            buffer[2 * i + 1] = (sample >> 8) & 0xFF; // Upper byte
        }
    }

    void initializeWaveHeader() {
        waveHeader.lpData = reinterpret_cast<LPSTR>(buffer);
        waveHeader.dwBufferLength = bufferSize;
        waveHeader.dwFlags = 0;
        waveHeader.dwLoops = 0;
    }

    void initializeWaveOut() {
        WAVEFORMATEX waveFormat;
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = numChannels;
        waveFormat.nSamplesPerSec = sampleRate;
        waveFormat.wBitsPerSample = bitsPerSample;
        waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

        MMRESULT result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL);
        if (result != MMSYSERR_NOERROR) {
            std::cerr << "Error opening audio device" << std::endl;
        }
    }

    void playSineWave() {
        std::cout << "Playing sine wave" << std::endl;
        waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));
    }

    void playSound(Sound sound) {
        printf("Playing sound: %s\n", sound.path);
        waveHeader.lpData = reinterpret_cast<LPSTR>(sound.wave.data());
        waveHeader.dwBufferLength = sound.wave.size();
        waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));
    }
};