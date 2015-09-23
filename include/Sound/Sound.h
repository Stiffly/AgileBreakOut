#ifndef SOUND_H__
#define SOUND_H__

#include "AL/al.h"
#include "AL/alc.h"

#include "Core/ResourceManager.h"

namespace dd
{

class Sound : public Resource
{
    friend class ResourceManager;
public:
    ALuint Buffer() { return m_Buffer; };
private:
    Sound(std::string path);

    //File-info
    char m_Type[4];
    unsigned long m_Size, m_ChunkSize;
    short m_FormatType, m_Channels;
    unsigned long m_SampleRate, m_AvgBytesPerSec;
    short m_BytesPerSample, m_BitsPerSample;
    unsigned long m_DataSize;
    std::map<std::string, ALuint> m_BufferCache;
    ALuint LoadFile(std::string path);

    ALuint m_Buffer;
};


}

#endif
