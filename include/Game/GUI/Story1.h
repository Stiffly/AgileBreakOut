#ifndef GAME_GUI_STORY1_H__
#define GAME_GUI_STORY1_H__

#include "Core/ResourceManager.h"
#include "Game/GUI/StoryboardFrame.h"
#include "Game/GUI/ELoadingFrameComplete.h"
#include "Game/GUI/LoadingFrame.h"
#include "Game/EGameStart.h"
#include "Core/EKeyDown.h"
#include "Game/GUI/HUD.h"

namespace dd
{
namespace GUI
{

class Story1 : public StoryboardFrame
{
public:
	Story1(Frame* parent, std::string name)
		: StoryboardFrame(parent, name) 
	{
		EVENT_SUBSCRIBE_MEMBER(m_ELoadingFrameComplete, &Story1::OnLoadingFrameComplete);
		EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &Story1::OnKeyDown);

		m_LoadingFrame = new GUI::LoadingFrame(this, "Story1LoadingFrame");
		m_LoadingFrame->SetTexture("Textures/GUI/Loading.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/Brunn1.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/Brunn2.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/Brunn3.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/Brunn4.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/Brunn5.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene1.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene2.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene3.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene4.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene5.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene6.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene7.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene8.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene9.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene10.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene11.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene12.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene13.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene14.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene15.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene16.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene17.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene18.png");
		m_LoadingFrame->AddTexturePreload("Textures/Story/SubScene19.png");
		m_LoadingFrame->Preload();

		m_Spacebar = new TextureFrame(this, "ZZZSpacebar");
		m_Spacebar->SetTexture("Textures/GUI/SpacebarSkip.png");
		m_Spacebar->SetRight(Right() - 10);
		m_Spacebar->SetBottom(Bottom() - 10);
		m_Spacebar->DisableScissor();

		Transition t;

		/*
			Sid in the well scene
		*/

		t.Time = t.Time + 0;
		t.Duration = 0;
		t.Texture = "Textures/Story/Brunn1.png";
		t.Viewport = Rectangle(3108, 11276, 1376, 2203);
		AddTransition(t);

		t.Time = t.Time + 2;
		t.Duration = 0.5;
		t.Texture = "Textures/Story/Brunn2.png";
		AddTransition(t);

		t.Time = t.Time + 3;
		t.Duration = 0.5;
		t.Texture = "Textures/Story/Brunn3.png";
		AddTransition(t);

		t.Time = t.Time + 3;
		t.Duration = 4;
		t.Texture = "Textures/Story/Brunn4.png";
		t.Viewport = Rectangle(2232, 0, 2952, 4725);
		AddTransition(t);

		t.Time = t.Time + 5;
		t.Duration = 0;
		t.Texture = "Textures/Story/Brunn5.png";
		AddTransition(t);

		/*
			Submarine scene
		*/

		t.Time = t.Time + 4;
		t.Duration = 2.5;
		t.Texture = "Textures/Story/SubScene1.png";
		t.Viewport = Rectangle(976, 0, 1615, 2586);
		AddTransition(t);

		// I'm Tom and stuff!
		t.Time = t.Time + 1;
		t.Duration = 0.5;
		t.Texture = "Textures/Story/SubScene2.png";
		AddTransition(t);

		// Pan to the right here
		t.Time = t.Time + 3.5;
		t.Duration = 2;
		t.Texture = "Textures/Story/SubScene3.png";
		t.Viewport = Rectangle(1574, 0, 1615, 2586);
		AddTransition(t);

		// ZAP!
		t.Time = t.Time + 2.5;
		t.Duration = 0.5;
		t.Texture = "Textures/Story/SubScene4.png";
		AddTransition(t);

		t.Time = t.Time + 0.5;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene5.png";
		AddTransition(t);

		t.Time = t.Time + 0.1;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene6.png";
		AddTransition(t);

		t.Time = t.Time + 0.5;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene7.png";
		AddTransition(t);

		// BZZT!
		t.Time = t.Time + 1;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene8.png";
		AddTransition(t);

		t.Time = t.Time + 0.5;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene9.png";
		AddTransition(t);

		t.Time = t.Time + 0.1;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene10.png";
		AddTransition(t);

		t.Time = t.Time + 0.5;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene11.png";
		AddTransition(t);

		t.Time = t.Time + 1;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene12.png";
		AddTransition(t);

		t.Time = t.Time + 0.5;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene13.png";
		AddTransition(t);

		t.Time = t.Time + 0.2;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene14.png";
		AddTransition(t);

		t.Time = t.Time + 0.5;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene15.png";
		AddTransition(t);

		t.Time = t.Time + 1;
		t.Duration = 0.5;
		t.Texture = "Textures/Story/SubScene16.png";
		AddTransition(t);

		t.Time = t.Time + 3.5;
		t.Duration = 0.5;
		t.Texture = "Textures/Story/SubScene17.png";
		AddTransition(t);

		t.Time = t.Time + 3;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene18.png";
		AddTransition(t);

		t.Time = t.Time + 2;
		t.Duration = 0;
		t.Texture = "Textures/Story/SubScene19.png";
		AddTransition(t);
	}

	void Update(double dt) override
	{
		StoryboardFrame::Update(dt);

		if (m_Spacebar == nullptr) {
		}
	}
	void OnComplete() override
	{
		m_Spacebar->Show();
	}

private:
	GUI::LoadingFrame* m_LoadingFrame = nullptr;
	TextureFrame* m_Spacebar = nullptr;

	EventRelay<Frame, Events::KeyDown> m_EKeyDown;
	bool OnKeyDown(const Events::KeyDown& event)
	{
		if (event.KeyCode == GLFW_KEY_SPACE) {
			m_LoadingFrame->CancelPreload();
			Events::GameStart e;
			EventBroker->Publish(e);
			Hide();
			auto hud = new GUI::HUD(BaseFrame, "HUD");
		}
		return true;
	}

	EventRelay<Frame, Events::LoadingFrameComplete> m_ELoadingFrameComplete;
	bool OnLoadingFrameComplete(const Events::LoadingFrameComplete& event)
	{
		if (event.FrameName == "Story1LoadingFrame") {
			event.Frame->Hide();
			m_Spacebar->Hide();
			Play();
		}
		return true;
	}
};

}
}

#endif