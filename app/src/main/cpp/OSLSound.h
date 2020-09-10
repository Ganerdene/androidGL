#ifndef OSL_SOUND_H
#define OSL_SOUND_H

#include <map>
#include <stddef.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

struct WAVHeader {
    char                RIFF[4];
    unsigned long       ChunkSize;
    char                WAVE[4];
    char                fmt[4];
    unsigned long       Subchunk1Size;
    unsigned short      AudioFormat;
    unsigned short      NumOfChan;
    unsigned long       SamplesPerSec;
    unsigned long       bytesPerSec;
    unsigned short      blockAlign;
    unsigned short      bitsPerSample;
    char                Subchunk2ID[4];
    unsigned long       Subchunk2Size;
};

struct SoundBuffer {
    WAVHeader* header = nullptr;
    char* buffer = nullptr;
    int length = 0;
};

struct ResourceDescriptor{
    int32_t descriptor = 0;
    off_t start = 0;
    off_t length = 0;
};

struct SoundPack {
    ResourceDescriptor descriptor;
    SLObjectItf player = nullptr;
    SLPlayItf play = nullptr;
    SLSeekItf seek = nullptr;
};

class OSLSound {

    public:

        static const enum SOUND_TYPE {
            BACKGROUND = 0,
            DESTROYED = 1,
            SOLID = 2,
            POWER_UP = 3
        };


    private:

        static const SLuint32 NUM_OPTIONS;
        static const SLuint32 NUM_INTERFACES;
        static const SLInterfaceID INTERFACE_ID[1];
        static const SLboolean INTERFACE_REQUIRED[1];
        static const SLuint32 OUTPUT_MIX_ID_COUNT;
        static const SLInterfaceID OUTPUT_MIX_IDS[0];
        static const SLboolean OUTPUT_MIX_REQUIRED[0];

        AAssetManager * m_pAssetManager;

        SLObjectItf m_oEngine;
        SLEngineItf m_oInterface;
        SLObjectItf m_oMix;

        std::map<SOUND_TYPE, SoundPack> m_oSoundPack;

        bool m_bIsSoundOn;


    public:

        OSLSound(AAssetManager *asset_manager, bool isSoundOn);

        ~OSLSound();


    public:

        void setSound(bool isSoundOn) {
            m_bIsSoundOn = isSoundOn;
        }

        bool isSoundOn() {
            return m_bIsSoundOn;
        }

        void play(SOUND_TYPE soundType);

        void stop(SOUND_TYPE soundType);

        void pauseAll();

        void stopAll();

        void pauseStopAll();


    private:

        bool init(AAssetManager *asset_manager);

        void create();

        void destroy(SLObjectItf* object);

        SLuint32 createAudioPlayer(SoundPack& soundPack);

        SoundBuffer * loadSoundFile(const char * filename);

        ResourceDescriptor loadResourceDescriptor(const char* path);
};

#endif
