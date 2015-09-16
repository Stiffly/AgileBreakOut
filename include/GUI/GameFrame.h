//
// Created by Administrator on 2015-09-16.
//

#ifndef DAYDREAM_GAMEFRAME_H
#define DAYDREAM_GAMEFRAME_H

#include "GUI/Frame.h"
#include "GUI/MainMenuFrame.h"
//#include "GUI/WorldFrame.h"
//#include "GUI/Viewport.h"
#include "GUI/TextureFrame.h"
//#include "GUI/PlayerHUD.h"

//#include "GameWorld.h"
#include "Game/EGameStart.h"
//#include "Events/GameOver.h"
//#include "Events/Dead.h"

namespace dd
{

    class GameFrame : public Frame
    {
    public:
        GameFrame(Frame* parent, std::string name)
                : Frame(parent, name)
                , m_GameOver(false)
        {
            EVENT_SUBSCRIBE_MEMBER(m_EGameStart, &GUI::GameFrame::OnGameStart);
            EVENT_SUBSCRIBE_MEMBER(m_EGameOver, &GUI::GameFrame::OnGameOver);

            //m_World = std::make_shared<GameWorld>(EventBroker, ResourceManager);
            /*m_WorldFrame = new WorldFrame(this, "GameWorldFrame", m_World);
            {
                vp1 = new Viewport(m_WorldFrame, "Viewport1", m_World);
                vp1->X = 0;
                vp1->Width = this->Width / 2.f;
                new PlayerHUD(vp1, "PlayerHUD", m_World, 1);

                vp2 = new Viewport(m_WorldFrame, "Viewport2", m_World);
                vp2->X = vp1->Right();
                vp2->Width = this->Width / 2.f;
                new PlayerHUD(vp2, "PlayerHUD", m_World, 2);

                m_FreeCamViewport = new Viewport(m_WorldFrame, "ViewportFreeCam", m_World);
                m_FreeCamViewport->Hide();
            }*/
            m_WorldFrame->Hide();
            m_World->Initialize();

            m_MainMenuFrame = new MainMenuFrame(this, "MainMenu");
        }

    private:
        EventRelay<Frame, dd::Events::GameStart> m_EGameStart;
        bool OnGameStart(const dd::Events::GameStart &event)
        {
            m_WorldFrame->Show();

            return true;
        }
        /*EventRelay<Frame, Events::GameOver> m_EGameOver;
        bool OnGameOver(const Events::GameOver &event)
        {
            m_GameOver = true;

            return true;
        }

        std::shared_ptr<GameWorld> m_World;

        MainMenuFrame* m_MainMenuFrame;
        WorldFrame* m_WorldFrame;
        Viewport* vp1;
        Viewport* vp2;
        Viewport* m_FreeCamViewport;

        bool m_GameOver;*/

        bool OnKeyUp(const dd::Events::KeyUp &event) override
        {
            if (event.KeyCode == GLFW_KEY_1)
            {
                /*if (m_FreeCamViewport->Hidden())
                    m_FreeCamViewport->Show();
                else
                    m_FreeCamViewport->Hide();*/
            }

            return true;
        }
    };

}

#endif //DAYDREAM_GAMEFRAME_H
