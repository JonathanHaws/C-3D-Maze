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
    WAVEFORMATEX waveFormat;
    WAVEHDR waveHeader;
    const int durationInSeconds = 2; // Duration of the sine wave in seconds
    const int sampleRate = 44100; // Sample rate (samples per second)
    const int numChannels = 2; // Number of channels
    const int bitsPerSample = 16; // Bits per sample
    const int bufferSize = sampleRate * numChannels * bitsPerSample / 8 * durationInSeconds; // Total size of the buffer
    BYTE* buffer; // Buffer to store the audio data

    Audio() {
        buffer = new BYTE[bufferSize];
        generateSawtoothWave();
        initializeWaveFormat();
        initializeWaveHeader();
        initializeWaveOut();
    }

    ~Audio() {
        delete[] buffer;
        waveOutClose(hWaveOut);
    }

    static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        if (uMsg == WOM_DONE) {
            Audio* audio = reinterpret_cast<Audio*>(dwInstance);
            audio->generateSawtoothWave();
            waveOutPrepareHeader(hwo, &(audio->waveHeader), sizeof(WAVEHDR));
            waveOutWrite(hwo, &(audio->waveHeader), sizeof(WAVEHDR));
        }
    }

    void generateSawtoothWave() {
        const double amplitude = 500.0; // Adjusted amplitude to make it less loud
        const double twoPi = 2.0 * 3.14159265358979323846;

        // Define the frequency of the sawtooth wave
        const double frequency = 220.0; // Frequency of the sawtooth wave

        for (int i = 0; i < bufferSize / 2; ++i) {
            double time = static_cast<double>(i) / sampleRate;

            // Calculate the value of the sawtooth wave at the current time
            double value = 0.0;
            for (int harmonic = 1; harmonic <= 10; ++harmonic) { // Adding harmonics for a richer sound
                value += amplitude / harmonic * sin(twoPi * frequency * time * harmonic);
            }

            // Assign the sample to the buffer (without averaging as it's not needed for a sawtooth wave)
            short sample = static_cast<short>(value);

            // Adjust the sample amplitude to fit within the 16-bit range
            if (sample > 32767) {
                sample = 32767;
            } else if (sample < -32768) {
                sample = -32768;
            }

            // Assign the sample to the buffer
            buffer[2 * i] = sample & 0xFF; // Lower byte
            buffer[2 * i + 1] = (sample >> 8) & 0xFF; // Upper byte
        }
    }

    void initializeWaveFormat() {
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = numChannels;
        waveFormat.nSamplesPerSec = sampleRate;
        waveFormat.wBitsPerSample = bitsPerSample;
        waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    }

    void initializeWaveHeader() {
        waveHeader.lpData = reinterpret_cast<LPSTR>(buffer);
        waveHeader.dwBufferLength = bufferSize;
        waveHeader.dwFlags = 0;
        waveHeader.dwLoops = 0;
    }

    void initializeWaveOut() {
        MMRESULT result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, reinterpret_cast<DWORD_PTR>(waveOutProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
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
        // waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        // waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));
    }
};