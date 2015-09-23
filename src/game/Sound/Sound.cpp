#include "Sound/Sound.h"

dd::Sound::Sound(std::string path)
{
    m_Buffer = LoadFile(path);
    m_Path = path;
}

ALuint dd::Sound::LoadFile(std::string path)
{
    if (m_BufferCache.find(path) != m_BufferCache.end()) {
        return m_BufferCache[path];
    }

    //Open file
    FILE *fp = fopen(path.c_str(), "rb");
    if (!fp) {
        LOG_ERROR("Failed to open file %s, no such file exists", path.c_str());
        return 0;
    }

    //CHECK FOR VALID WAVE-FILE
    fread(m_Type, sizeof(char), 4, fp);
    if(m_Type[0] != 'R' || m_Type[1] != 'I' || m_Type[2] != 'F' || m_Type[3] != 'F') {
        LOG_ERROR("ERROR: No RIFF in WAVE-file");
        return 0;
    }

    fread(&m_Size, sizeof(unsigned long), 1, fp);
    fread(m_Type, sizeof(char), 4, fp);
    if (m_Type[0] != 'W' || m_Type[1] != 'A' || m_Type[2] != 'V' || m_Type[3]!= 'E') {
        LOG_ERROR("ERROR: Not WAVE-file");
        return 0;
    }

    fread(m_Type, sizeof(char), 4, fp);
    if (m_Type[0] != 'f' || m_Type[1] != 'm' || m_Type[2] != 't' || m_Type[3] != ' ') {
        LOG_ERROR("ERROR: No fmt in WAVE-file");
        return 0;
    }

    //READ THE DATA FROM WAVE-FILE
    fread(&m_ChunkSize, sizeof(unsigned long), 1, fp);
    fread(&m_FormatType, sizeof(short), 1, fp);
    fread(&m_Channels, sizeof(short), 1, fp);
    fread(&m_SampleRate, sizeof(unsigned long), 1, fp);
    fread(&m_AvgBytesPerSec, sizeof(unsigned long), 1, fp);
    fread(&m_BytesPerSample, sizeof(short), 1, fp);
    fread(&m_BitsPerSample, sizeof(short), 1, fp);

    fread(m_Type, sizeof(char), 4, fp);
    if (m_Type[0] !='d' || m_Type[1] != 'a' || m_Type[2] != 't' || m_Type[3] != 'a') {
        LOG_ERROR("ERROR: WAVE-file Missing data");
        return 0;
    }

    fread(&m_DataSize, sizeof(unsigned long), 1, fp);

    unsigned char* buf = new unsigned char[m_DataSize];
    fread(buf, sizeof(unsigned char), m_DataSize, fp);
    fclose(fp);

    // Create buffer
    ALuint format = 0;
    if (m_BitsPerSample == 8) {
        if (m_Channels == 1) {
            format = AL_FORMAT_MONO8;
        }
        else if (m_Channels == 2) {
            format = AL_FORMAT_STEREO8;
        }
    }
    if (m_BitsPerSample == 16) {
        if (m_Channels == 1) {
            format = AL_FORMAT_MONO16;
        }
        else if (m_Channels == 2) {
            format = AL_FORMAT_STEREO16;
        }
    }

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, buf, m_DataSize, m_SampleRate);
    delete[] buf;

    m_BufferCache[path] = buffer;
    return buffer;
}