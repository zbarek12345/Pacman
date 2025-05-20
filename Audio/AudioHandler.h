#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <queue>
#include <pthread.h>

class AudioHandler {
public:
    // Sound effect enums (matches Pac-Man sounds)
    enum Sounds {
        INTRO,
        CUTSCENE,
        WAKA_WAKA,
        GHOST_SIREN,
        GHOST_EATEN,
        POWER_PELLET_EATEN,
        GHOST_RETURN,
        PAC_MAN_DEATH,
        HIGH_SCORE,
        POWER_UP
    };

    // Constructor and destructor
    AudioHandler();
    ~AudioHandler();

    // Load a sound file and associate it with a sound type
    void loadSound(Sounds type, const char* path, bool loop);

    // Trigger a sound to play (called by game logic)
    void playSound(Sounds type);

    // Stop a specific sound
    void stopSound(Sounds type);

    // Stop all sounds
    void stopAllSounds();

private:
    // Sound state for each effect
    struct SoundEvent {
        Sounds type;             // Which sound (e.g., WAKA_WAKA)
        bool isPlaying;          // Is the sound currently playing?
        bool loop;               // Should the sound loop? (e.g., GHOST_SIREN)
        int channel;             // Mixer channel assigned (-1 if not playing)
        Mix_Chunk* chunk;        // Sound data (WAV)

        SoundEvent(Sounds t, Mix_Chunk* c, bool l)
            : type(t), isPlaying(false), loop(l), channel(-1), chunk(c) {}
    };

    // Global variables with _{name} convention
    std::unordered_map<Sounds, SoundEvent> _soundEvents; // Sound states
    std::queue<Sounds> _eventQueue;                     // Queue for sound triggers
    pthread_mutex_t _mutex;                             // Thread safety
    bool _running;                                      // Thread control
    pthread_t _audioThread;                             // Audio thread

    // Audio thread function
    static void* audioThreadLoop(void* arg);

    // Prevent copying
    AudioHandler(const AudioHandler&);
    AudioHandler& operator=(const AudioHandler&);
};

#endif // AUDIO_HANDLER_H