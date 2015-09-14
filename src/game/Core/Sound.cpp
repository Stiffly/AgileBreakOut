//
// Created by Adam on 2015-09-09.
//
#include "PrecompiledHeader.h"
#include "Core/Sound.h"

void dd::Systems::Sound::Initialize()
{
    LOG_INFO("Hello im an init func ----------------------------");
    /*
    LOG_ERROR("Init started");

    //initialize OpenAL
    ALCdevice* device = alcOpenDevice(NULL);
    ALCcontext* context;
    if(device)
        {
        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
    }
    else
    {
        LOG_ERROR("OMG OPEN AL FAIL");
    }

    alGetError();

    alSpeedOfSound(340.29f); // Speed of sound m/s
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    //Subscribe to events
    EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &Sound::OnKeyDown);

    LOG_ERROR("Init complete");

    */
}

void dd::Systems::Sound::Update(double dt)
{
    /*const ALfloat pos[3] = {0, 0, 0};
    alListenerfv(AL_POSITION, pos);

    alSourcefv(m_source, AL_POSITION, pos);
*/

}

bool dd::Systems::Sound::OnKeyDown(const dd::Events::KeyDown &event)
{
    // #define GLFW_KEY_S 83
    if(event.KeyCode == 83){
        m_source = CreateSource();

        ALuint buffer = LoadFile("assets/Sounds/screwed.wav");

        alSourcei(m_source, AL_BUFFER, buffer);
        alSourcePlay(m_source);


        LOG_INFO("Playing sound (I wish LOL)");
    }
}

ALuint dd::Systems::Sound::CreateSource()
{
    ALuint source;
    alGenSources((ALuint)1, &source);

    return source;
}

ALuint dd::Systems::Sound::LoadFile(std::string path)
{
    if (m_BufferCache.find(path) != m_BufferCache.end())
        return m_BufferCache[path];

    FILE *fp = NULL;
    fp = fopen(path.c_str(), "rb");

    //CHECK FOR VALID WAVE-FILE
    fread(m_type, sizeof(char), 4, fp);
    if(m_type[0]!='R' || m_type[1]!='I' || m_type[2]!='F' || m_type[3]!='F') {
        LOG_ERROR("ERROR: No RIFF in WAVE-file");
        return 0;
    }

    fread(&m_size, sizeof(unsigned long), 1, fp);
    fread(m_type, sizeof(char), 4, fp);
    if(m_type[0]!='W' || m_type[1]!='A' || m_type[2]!='V' || m_type[3]!='E') {
        LOG_ERROR("ERROR: Not WAVE-file");
        return 0;
    }

    fread(m_type, sizeof(char), 4, fp);
    if(m_type[0]!='f' || m_type[1]!='m' || m_type[2]!='t' || m_type[3]!=' ') {
        LOG_ERROR("ERROR: No fmt in WAVE-file");
        return 0;
    }

    //READ THE DATA FROM WAVE-FILE
    fread(&m_chunkSize, sizeof(unsigned long), 1, fp);
    fread(&m_formatType, sizeof(short), 1, fp);
    fread(&m_channels, sizeof(short), 1, fp);
    fread(&m_sampleRate, sizeof(unsigned long), 1, fp);
    fread(&m_avgBytesPerSec, sizeof(unsigned long), 1, fp);
    fread(&m_bytesPerSample, sizeof(short), 1, fp);
    fread(&m_bitsPerSample, sizeof(short), 1, fp);

    fread(m_type, sizeof(char), 4, fp);
    if(m_type[0]!='d' || m_type[1]!='a' || m_type[2]!='t' || m_type[3]!='a')
    {
        LOG_ERROR("ERROR: WAVE-file Missing data");
        return 0;
    }

    fread(&m_dataSize, sizeof(unsigned long), 1, fp);

    unsigned char* buf = new unsigned char[m_dataSize];
    fread(buf, sizeof(unsigned char), m_dataSize, fp);
    fclose(fp);

    // Create buffer
    ALuint format = 0;
    if(m_bitsPerSample == 8)
    {
        if(m_channels == 1)
            format = AL_FORMAT_MONO8;
        else if(m_channels == 2)
            format = AL_FORMAT_STEREO8;
    }
    if(m_bitsPerSample == 16)
    {
        if (m_channels == 1)
            format = AL_FORMAT_MONO16;
        else if (m_channels == 2)
            format = AL_FORMAT_STEREO16;
    }

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, buf, m_dataSize, m_sampleRate);
    delete[] buf;

    m_BufferCache[path] = buffer;
    return buffer;
}
