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
        const double amplitude = 32767.0; // Amplitude of the sine wave (maximum value for 16-bit audio)
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

    void generate_sine_wave(int seconds) {

    }

    Sound() {
        //generate_sine_wave();
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
    const int bufferSize = 64 * 2 * 16 / 8; // buffer size in bytes
    BYTE* buffer; // Buffer to store the audio data

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
    }



    static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        if (uMsg == WOM_DONE) {
            Audio* audio = reinterpret_cast<Audio*>(dwInstance);
            
            // Fill the buffer with random noise
            for (int i = 0; i < audio->bufferSize; ++i) {
                short sample = static_cast<short>(rand() % 65536 - 32768);
                audio->buffer[i] = sample & 0xFF; // Lower byte
            }

            waveOutPrepareHeader(hwo, &(audio->waveHeader), sizeof(WAVEHDR));
            waveOutWrite(hwo, &(audio->waveHeader), sizeof(WAVEHDR));
        }
    }

};