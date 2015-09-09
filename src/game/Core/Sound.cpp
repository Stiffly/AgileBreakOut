//
// Created by Adam on 2015-09-09.
//
#include "PrecompiledHeader.h"
#include "Core/Sound.h"

void dd::Systems::Sound::Init()
{

}

void dd::Systems::Sound::HelloWorld()
{
    LOG_INFO("Hej världen!");
}

bool dd::Systems::Sound::OnKeyDown(const dd::Events::KeyDown &event)
{
    // #define GLFW_KEY_S 83
    if(event.KeyCode == 83){
        HelloWorld();
    }
}
