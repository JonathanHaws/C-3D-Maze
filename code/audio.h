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
        generateSineWave();
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
            audio->generateSineWave();
            waveOutPrepareHeader(hwo, &(audio->waveHeader), sizeof(WAVEHDR));
            waveOutWrite(hwo, &(audio->waveHeader), sizeof(WAVEHDR));
        }
    }

    void generateSineWave() {
    const double amplitude = 32767.0; // Amplitude of the sine wave (maximum value for 16-bit audio)
    const double twoPi = 2.0 * 3.14159265358979323846;

    // Define the frequencies for the two sine waves (A4 and E5 for a perfect fifth)
    const double frequencyA = 440.0; // Frequency of the A4 note
    const double frequencyE = frequencyA * (3.0 / 2.0); // Frequency of the E5 note (perfect fifth)

    for (int i = 0; i < bufferSize / 2; ++i) {
        double time = static_cast<double>(i) / sampleRate;

        // Calculate the values of the two sine waves at the current time
        double valueA = amplitude * sin(twoPi * frequencyA * time);
        double valueE = amplitude * sin(twoPi * frequencyE * time);

        // Combine the values of the two sine waves
        short sample = static_cast<short>((valueA + valueE) / 2.0); // Average the values to avoid clipping

        // Assign the combined sample to the buffer
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