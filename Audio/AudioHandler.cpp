#include "AudioHandler.h"
#include <string.h>
#include <stdio.h>

AudioHandler::AudioHandler() : _running(true) {
    // Initialize SDL audio and SDL_mixer
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL audio init failed: %s\n", SDL_GetError());
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer init failed: %s\n", Mix_GetError());
        return;
    }

    // Allocate channels (8 is enough for Pac-Man-like games)
    Mix_AllocateChannels(8);

    // Initialize mutex
    if (pthread_mutex_init(&_mutex, NULL) != 0) {
        fprintf(stderr, "Mutex init failed\n");
        Mix_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }

    // Start audio thread
    if (pthread_create(&_audioThread, NULL, audioThreadLoop, this) != 0) {
        fprintf(stderr, "Audio thread creation failed\n");
        pthread_mutex_destroy(&_mutex);
        Mix_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
}

AudioHandler::~AudioHandler() {
    // Stop audio thread
    _running = false;
    pthread_join(_audioThread, NULL);
    pthread_mutex_destroy(&_mutex);

    // Free sound chunks
    for (auto& pair : _soundEvents) {
        if (pair.second.chunk) {
            Mix_FreeChunk(pair.second.chunk);
        }
    }

    // Clean up SDL_mixer and SDL audio
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioHandler::loadSound(Sounds type, const char* path, bool loop) {
    Mix_Chunk* chunk = Mix_LoadWAV(path);
    if (!chunk) {
        fprintf(stderr, "Failed to load sound %s: %s\n", path, Mix_GetError());
        return;
    }

    pthread_mutex_lock(&_mutex);
    _soundEvents.insert({type, SoundEvent(type, chunk, loop)});
    pthread_mutex_unlock(&_mutex);
}

void AudioHandler::playSound(Sounds type) {
    pthread_mutex_lock(&_mutex);
    _eventQueue.push(type);
    pthread_mutex_unlock(&_mutex);
}

void AudioHandler::stopSound(Sounds type) {
    pthread_mutex_lock(&_mutex);
    auto it = _soundEvents.find(type);
    if (it != _soundEvents.end() && it->second.isPlaying) {
        Mix_HaltChannel(it->second.channel);
        it->second.isPlaying = false;
        it->second.channel = -1;
    }
    pthread_mutex_unlock(&_mutex);
}

void AudioHandler::stopAllSounds() {
    pthread_mutex_lock(&_mutex);
    Mix_HaltChannel(-1); // Halt all channels
    for (auto& pair : _soundEvents) {
        pair.second.isPlaying = false;
        pair.second.channel = -1;
    }
    pthread_mutex_unlock(&_mutex);
}

void* AudioHandler::audioThreadLoop(void* arg) {
    AudioHandler* handler = static_cast<AudioHandler*>(arg);

    while (handler->_running) {
        Sounds type;
        bool hasEvent = false;

        // Check for new sound events
        pthread_mutex_lock(&handler->_mutex);
        if (!handler->_eventQueue.empty()) {
            type = handler->_eventQueue.front();
            handler->_eventQueue.pop();
            hasEvent = true;
        }
        pthread_mutex_unlock(&handler->_mutex);

        if (hasEvent) {
            pthread_mutex_lock(&handler->_mutex);
            auto it = handler->_soundEvents.find(type);
            if (it != handler->_soundEvents.end()) {
                SoundEvent& event = it->second;

                // Stop any existing playback of this sound
                if (event.isPlaying) {
                    Mix_HaltChannel(event.channel);
                }

                // Play the sound
                event.channel = Mix_PlayChannel(-1, event.chunk, event.loop ? -1 : 0);
                if (event.channel != -1) {
                    event.isPlaying = true;
                }
            }
            pthread_mutex_unlock(&handler->_mutex);
        }

        // Check for finished sounds
        pthread_mutex_lock(&handler->_mutex);
        for (auto& pair : handler->_soundEvents) {
            SoundEvent& event = pair.second;
            if (event.isPlaying && !Mix_Playing(event.channel)) {
                event.isPlaying = false;
                event.channel = -1;
            }
        }
        pthread_mutex_unlock(&handler->_mutex);

        // Avoid busy-waiting
        SDL_Delay(10);
    }

    return NULL;
}