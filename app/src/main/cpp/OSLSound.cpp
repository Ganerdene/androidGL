#include "OSLSound.h"
#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

const SLuint32 OSLSound::NUM_OPTIONS = 0;
const SLuint32 OSLSound::NUM_INTERFACES = 1;
const SLInterfaceID OSLSound::INTERFACE_ID[1] = { SL_IID_ENGINE };
const SLboolean OSLSound::INTERFACE_REQUIRED[1]  = { SL_BOOLEAN_TRUE };
const SLuint32 OSLSound::OUTPUT_MIX_ID_COUNT = 0;
const SLInterfaceID OSLSound::OUTPUT_MIX_IDS[0] = {};
const SLboolean OSLSound::OUTPUT_MIX_REQUIRED[0] = {};

OSLSound::OSLSound(AAssetManager *asset_manager, bool isSoundOn) : m_bIsSoundOn(isSoundOn) {
    LOGI("OSLSound::OSLSound()");
    if (init(asset_manager)) {
        create();
    }
}

OSLSound::~OSLSound() {
    LOGI("OSLSound::~OSLSound()");

    std::for_each(m_oSoundPack.begin(), m_oSoundPack.end(), [&](std::map<SOUND_TYPE, SoundPack>::value_type &item) {
        if (item.second.play) {
            (*item.second.play)->SetPlayState(item.second.play, SL_PLAYSTATE_STOPPED);
            item.second.play = nullptr;
            item.second.seek = nullptr;
        }
        destroy(&item.second.player);
    });

    destroy(&m_oMix);
    destroy(&m_oEngine);
};

bool OSLSound::init(AAssetManager *asset_manager) {
    // Get object for required interfaces
    SLresult result = slCreateEngine(&m_oEngine, NUM_OPTIONS, NULL, NUM_INTERFACES, INTERFACE_ID, INTERFACE_REQUIRED);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound - Error slCreateEngine");
        return false;
    }

    // Allocates resources
    result = (*m_oEngine)->Realize(m_oEngine, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound - Error m_oEngine Realize");
        return false;
    }

    // Get interfaces for output access
    result = (*m_oEngine)->GetInterface(m_oEngine, SL_IID_ENGINE, &m_oInterface);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound - Error GetInterface");
        return false;
    }

    // Get object for device audio output
    result = (*m_oInterface)->CreateOutputMix(m_oInterface, &m_oMix, OUTPUT_MIX_ID_COUNT, OUTPUT_MIX_IDS, OUTPUT_MIX_REQUIRED);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound - Error CreateOutputMix");
        return false;
    }

    // Allocates resources
    result = (*m_oMix)->Realize(m_oMix, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound - Error m_oMix Realize");
        return false;
    }

    m_pAssetManager = asset_manager;
    return true;
}

void OSLSound::create() {
    LOGI("OSLSound::create()");
    m_oSoundPack[SOUND_TYPE::BACKGROUND] = SoundPack();
    m_oSoundPack[SOUND_TYPE::BACKGROUND].descriptor = loadResourceDescriptor("sound/breakout.mp3");

    m_oSoundPack[SOUND_TYPE::DESTROYED] = SoundPack();
    m_oSoundPack[SOUND_TYPE::DESTROYED].descriptor = loadResourceDescriptor("sound/bleep.mp3");

    m_oSoundPack[SOUND_TYPE::SOLID] = SoundPack();
    m_oSoundPack[SOUND_TYPE::SOLID].descriptor = loadResourceDescriptor("sound/solid.wav");

    m_oSoundPack[SOUND_TYPE::POWER_UP] = SoundPack();
    m_oSoundPack[SOUND_TYPE::POWER_UP].descriptor = loadResourceDescriptor("sound/powerup.wav");
//    m_oSoundPack[SOUND_TYPE::BALL] = SoundPack();
//    m_oSoundPack[SOUND_TYPE::BALL].descriptor = loadResourceDescriptor("sound/ball.mp3");
//    m_oSoundPack[SOUND_TYPE::OUT] = SoundPack();
//    m_oSoundPack[SOUND_TYPE::OUT].descriptor = loadResourceDescriptor("sound/out.mp3");

    std::for_each(m_oSoundPack.begin(), m_oSoundPack.end(), [&](std::map<SOUND_TYPE, SoundPack>::value_type &item) {
        createAudioPlayer(item.second);
    });

    (*m_oSoundPack[SOUND_TYPE::BACKGROUND].seek)->SetLoop(m_oSoundPack[SOUND_TYPE::BACKGROUND].seek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    LOGI("OSLSound::create()___End");
};

void OSLSound::play(OSLSound::SOUND_TYPE soundType) {
    if (m_bIsSoundOn) {
        if (soundType != SOUND_TYPE::BACKGROUND) {
            (*m_oSoundPack[soundType].play)->SetPlayState(m_oSoundPack[soundType].play, SL_PLAYSTATE_STOPPED);
        }
        (*m_oSoundPack[soundType].play)->SetPlayState(m_oSoundPack[soundType].play, SL_PLAYSTATE_PLAYING);
    }
};

void OSLSound::stop(OSLSound::SOUND_TYPE soundType) {
    if (m_bIsSoundOn) {
        (*m_oSoundPack[soundType].play)->SetPlayState(m_oSoundPack[soundType].play, SL_PLAYSTATE_STOPPED);
    }
};

void OSLSound::pauseAll() {
    std::for_each(m_oSoundPack.begin(), m_oSoundPack.end(), [&](std::map<SOUND_TYPE, SoundPack>::value_type &item) {
        (*item.second.play)->SetPlayState(item.second.play, SL_PLAYSTATE_PAUSED);
    });
};

void OSLSound::stopAll() {
    std::for_each(m_oSoundPack.begin(), m_oSoundPack.end(), [&](std::map<SOUND_TYPE, SoundPack>::value_type &item) {
        (*item.second.play)->SetPlayState(item.second.play, SL_PLAYSTATE_STOPPED);
    });
};

void OSLSound::pauseStopAll() {
    pauseAll();
    stopAll();
}

SLuint32 OSLSound::createAudioPlayer(SoundPack& soundPack) {
    SLDataLocator_AndroidFD dataLocatorIn;
    dataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDFD;
    dataLocatorIn.fd = soundPack.descriptor.descriptor;
    dataLocatorIn.offset = soundPack.descriptor.start;
    dataLocatorIn.length = soundPack.descriptor.length;

    SLDataFormat_MIME dataFormat;
    dataFormat.formatType = SL_DATAFORMAT_MIME;
    dataFormat.mimeType = NULL;
    dataFormat.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

    SLDataSource dataSource;
    dataSource.pLocator = &dataLocatorIn;
    dataSource.pFormat = &dataFormat;

    SLDataLocator_OutputMix dataLocatorOut;
    dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    dataLocatorOut.outputMix = m_oMix;

    SLDataSink dataSink;
    dataSink.pLocator = &dataLocatorOut;
    dataSink.pFormat = NULL;

    const SLuint32 BGMPlayerIDCount = 2;
    const SLInterfaceID BGMPlayerIDs[] = { SL_IID_PLAY, SL_IID_SEEK };
    const SLboolean BGMPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

    SLresult result = (*m_oInterface)->CreateAudioPlayer(m_oInterface,
                                                         &soundPack.player,
                                                         &dataSource,
                                                         &dataSink,
                                                         BGMPlayerIDCount,
                                                         BGMPlayerIDs,
                                                         BGMPlayerReqs);

    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound::createAudioPlayer - can not create audio player %d", result);
        soundPack.player = nullptr;
        return result;
    }

    result = (*soundPack.player)->Realize(soundPack.player, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound::createAudioPlayer - can not realize player");
        soundPack.player = nullptr;
        return result;
    }

    result = (*soundPack.player)->GetInterface(soundPack.player, SL_IID_PLAY, &soundPack.play);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound::createAudioPlayer - can not get interface play");
        destroy(&soundPack.player);
        soundPack.play = nullptr;
        return result;
    }

    result = (*soundPack.player)->GetInterface(soundPack.player, SL_IID_SEEK, &soundPack.seek);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("OSLSound::createAudioPlayer - can not get interface seek");
        destroy(&soundPack.player);
        soundPack.seek = nullptr;
        return result;
    }

    (*soundPack.seek)->SetLoop(soundPack.seek, SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN);
    (*soundPack.play)->SetPlayState(soundPack.play, SL_PLAYSTATE_STOPPED);

    return SL_RESULT_SUCCESS;
}

SoundBuffer * OSLSound::loadSoundFile(const char * filename) {
    AAsset* asset = AAssetManager_open(m_pAssetManager, filename, AASSET_MODE_UNKNOWN);
    off_t length = AAsset_getLength(asset);

    SoundBuffer* result = new SoundBuffer();
    result->length = length - sizeof(WAVHeader);
    result->header = new WAVHeader();
    result->buffer = new char[result->length];

    AAsset_read(asset, result->header, sizeof(WAVHeader));
    AAsset_read(asset, result->buffer, result->length);
    AAsset_close(asset);
    return result;
}

ResourceDescriptor OSLSound::loadResourceDescriptor(const char * path) {
    AAsset * asset = AAssetManager_open(m_pAssetManager, path, AASSET_MODE_UNKNOWN);
    ResourceDescriptor resourceDescriptor;
    resourceDescriptor.descriptor = AAsset_openFileDescriptor(asset, &resourceDescriptor.start, &resourceDescriptor.length);
    AAsset_close(asset);
    return resourceDescriptor;
}

void OSLSound::destroy(SLObjectItf* object) {
    (**object)->Destroy(*object);
    *object = nullptr;
}
