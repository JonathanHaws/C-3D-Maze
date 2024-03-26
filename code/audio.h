#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <vector>
#include <fstream>
#include <thread>

// Only supports 16-bit wave audio so far

struct Sound {

    const char* path;
    std::vector<BYTE> wave;
    int progress = 0;
    bool loop = false;

    void printData() {
        for (int i = 0; i < wave.size(); i++) { printf("%d ", wave[i]); }
    }

    Sound(int frequency = 440) {
        generate_sine_wave(1, 44100, frequency);
        //printData();
    }

    Sound(const char* path) {
        load_wave_file(path); 
        //printData();
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

    void load_wave_file(const char* path) {
        std::ifstream file(path, std::ios::binary);
        if(file) {
            wave.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
            //printf("Successfully read audio file: %s\n", path);
        } else {
            //printf("Error opening audio file: %s\n", path);
        }
        progress = wave.size();
    }

};

struct Audio {

    int buffers_played = 0;
    int current_buffer = 0;
    static const int number_of_buffers = 16;
    int needed_buffers = number_of_buffers; 
    static const int buffer_samples = 2048;
    static const int buffer_size = buffer_samples * 2 * 16 / 8; // 64 samples, 2 channels, 16 bits per sample, 8 bits per byte
    BYTE buffers[number_of_buffers][buffer_size]; 
    HWAVEOUT hWaveOut; // Plaform-specific handle to the audio device
    WAVEFORMATEX waveFormat;
    WAVEHDR waveHeader;
    std::thread audioThread;
    std::vector<Sound*> sounds; // Vector to hold pointers to Sound objects

    void play(Sound* sound) {
        sound->progress = 0;
        sounds.push_back(sound);
    }

    Audio() {
        //play_sound(Sound(440));
        //play_sound(Sound(660));
        //play_sound(Sound("audio/expand.wav"));
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2;
        waveFormat.nSamplesPerSec = 44100;
        waveFormat.wBitsPerSample = 16;
        waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
        waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, reinterpret_cast<DWORD_PTR>(waveOutProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);

        audioThread = std::thread(&Audio::tick, this);
        memset(buffers[current_buffer], 0, buffer_size);
    }

    ~Audio() {
        waveOutReset(hWaveOut);
        waveOutUnprepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutClose(hWaveOut);
        
        if (audioThread.joinable()) { audioThread.join(); }
    }
    
    static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        if (uMsg != WOM_DONE) { return; }
        Audio* audio = reinterpret_cast<Audio*>(dwInstance);
        audio->needed_buffers++;
    }

    void send_buffer_to_audio_device(BYTE* buffer) {
        waveHeader.lpData = reinterpret_cast<LPSTR>(buffer);
        waveHeader.dwBufferLength = buffer_size;
        waveHeader.dwFlags = 0;
        waveHeader.dwLoops = 0;
        waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));
    } 

    void tick() {
        while (true) {
            if (needed_buffers <= 0) { continue; }
            send_buffer_to_audio_device(buffers[current_buffer]);
            buffers_played++;
            needed_buffers--;
            current_buffer = (current_buffer + 1) % number_of_buffers;   
            memset(buffers[current_buffer], 0, buffer_size);

            mix();   
        }
    }

    void mix() {
        for (auto sound : sounds) {
            for (int i = 0; i < buffer_size; i++) { // Add the sample from the current sound to the current buffer 
                if (sound->loop) { sound->progress %= sound->wave.size(); }
                if (sound->progress < sound->wave.size()) { buffers[current_buffer][i] += sound->wave[sound->progress]; }
                sound->progress++;
            }
        }
    }

};