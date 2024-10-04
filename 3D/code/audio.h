#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <vector>
#include <fstream>
#include <thread>

// Only supports 16-bit wave audio so far

struct Sound {

    const char* path;
    int progress = 0;
    bool loop = false;

    std::vector<BYTE> wave;
    int sample_rate = 44100;
    int channels = 2;
    int bits_per_sample = 16;

    void print_data() {
        for (int i = 0; i < wave.size(); i++) { printf("%d ", wave[i]); }
    }

    void print_metadata() {
        std::cout << "Sample rate: " << sample_rate << std::endl;
        std::cout << "Channels: " << channels << std::endl;
        std::cout << "Bits per sample: " << bits_per_sample << std::endl;
        std::cout << "Size: " << wave.size() << std::endl;
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
            // Read WAV header
            char header[44]; // Read the first 44 bytes initially
            file.read(header, 44);

            // Check if it's a WAV file
            if (header[0] != 'R' || header[1] != 'I' || header[2] != 'F' || header[3] != 'F' ||
                header[8] != 'W' || header[9] != 'A' || header[10] != 'V' || header[11] != 'E') {
                std::cerr << "Not a valid WAV file: " << path << std::endl;
                return;
            }

            // Extract sample rate, number of channels, and bits per sample from header
            sample_rate = *reinterpret_cast<int*>(header + 24);
            channels = *reinterpret_cast<short*>(header + 22);
            bits_per_sample = *reinterpret_cast<short*>(header + 34);

            // Move file pointer to the beginning of the audio data (skip initial header)
            file.seekg(0, std::ios::end);
            std::streampos fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            // Calculate the size of the audio data by subtracting header size from file size
            int headerSize = 44; // Size of the initial header
            int audioDataSize = static_cast<int>(fileSize) - headerSize;

            // Read audio data
            wave.clear();
            wave.resize(audioDataSize);
            file.read(reinterpret_cast<char*>(wave.data()), audioDataSize);

            //print_metadata();
        } else {
            std::cerr << "Error opening audio file: " << path << std::endl;
        }
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

    void play(int sound) {
        sounds[sound]->progress = 0;
    }

    int load(const char* path) {
        Sound* sound = new Sound(path);
        sounds.push_back(sound);
        sounds[sounds.size() - 1]->progress = sounds[sounds.size() - 1]->wave.size();
        return sounds.size() - 1;
    }

    int load(int frequency) {
        Sound* sound = new Sound(frequency);
        sounds.push_back(sound);
        sounds[sounds.size() - 1]->progress = sounds[sounds.size() - 1]->wave.size();
        return sounds.size() - 1;
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
        for (int i = 0; i < buffer_size; i++) {
            float sample_sum = 0.0f;
            int contributing_sounds = 0;
            for (auto sound : sounds) {
                if (sound->loop) { sound->progress %= sound->wave.size(); }
                if (sound->progress >= sound->wave.size()) { continue; }
                sample_sum += static_cast<float>(sound->wave[sound->progress]);
                contributing_sounds++;
                sound->progress++;
            }
            if (contributing_sounds > 0) { sample_sum /= contributing_sounds; }
            sample_sum = static_cast<BYTE>(sample_sum);
            if (sample_sum < 0 || sample_sum > 255) { std::cout << "Sample sum out of range: " << sample_sum << std::endl;}
            buffers[current_buffer][i] = sample_sum;
        }
    }

};