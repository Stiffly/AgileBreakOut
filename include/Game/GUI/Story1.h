#ifndef GAME_GUI_STORY1_H__
#define GAME_GUI_STORY1_H__

#include "Core/ResourceManager.h"
#include "Game/GUI/StoryboardFrame.h"
#include "Game/EGameStart.h"
#include "Core/EKeyDown.h"
#include "Game/GUI/HUD.h"
#include "Sound/EPlaySound.h"
#include "Sound/EStopSound.h"

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
		EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &Story1::OnKeyDown);

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

	virtual void Play() override
	{
		StoryboardFrame::Play();
		Events::PlaySound se;
		se.FilePath = "Sounds/jap-story.wav";
		EventBroker->Publish(se);
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
	TextureFrame* m_Spacebar = nullptr;

	EventRelay<Frame, Events::KeyDown> m_EKeyDown;
	bool OnKeyDown(const Events::KeyDown& event)
	{
		if (Hidden()) {
			return false;
		}

		if (event.KeyCode == GLFW_KEY_SPACE) {
			Events::GameStart e;
			EventBroker->Publish(e);
			Events::StopSound se;
			se.FilePath = "Sounds/jap-story.wav";
			EventBroker->Publish(se);
			Hide();
			auto hud = new GUI::HUD(BaseFrame, "HUD");
		}
		return true;
	}

};

}
}

#endif