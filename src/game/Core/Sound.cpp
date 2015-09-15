/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung

	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PrecompiledHeader.h"
#include "Core/Sound.h"

dd::Systems::Sound::~Sound()
{
    //alDeleteSources(1, m_Source);
}

void dd::Systems::Sound::Initialize()
{   //initialize OpenAL
    ALCdevice* device = alcOpenDevice(NULL);
    ALCcontext* context;

    if (device) {
        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
    }
    else {
        LOG_ERROR("OpenAL failed to initialize.");
    }

    alGetError();
    alSpeedOfSound(340.29f); // Speed of sound m/s
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    //Subscribe to events
    EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &Sound::OnKeyDown);
    //EVENT_SUBSCRIBE_MEMBER(m_EPlaySound, &Sound::PlayASound);
}

void dd::Systems::Sound::Update(double dt)
{
    const ALfloat pos[3] = {0, 0, 0};
    alListenerfv(AL_POSITION, pos);

    alSourcefv(m_Source, AL_POSITION, pos);
}

/*bool dd::Systems::Sound::PlayASound(const dd::Events::PlaySound &event)
{

}*/

bool dd::Systems::Sound::OnKeyDown(const dd::Events::KeyDown &event)
{
    // #define GLFW_KEY_S 83
    /*if (event.KeyCode == 83) {
        Events::PlaySound e;
        e.path = "Sounds/screwed.wav";
        EventBroker->Publish(e);
    }*/
    if (event.KeyCode == 83) {
        m_Source = CreateSource();
        ALuint buffer = LoadFile("Sounds/screwed.wav");

        alSourcei(m_Source, AL_BUFFER, buffer);
        alSourcePlay(m_Source);
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
