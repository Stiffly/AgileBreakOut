#ifndef GUI_FPSCOUNTER_H__
#define GUI_FPSCOUNTER_H__

#include "GUI/Frame.h"
#include "GUI/NumberFrame.h"

namespace dd
{
namespace GUI
{

class FPSCounter : public Frame
{
public:
	FPSCounter(Frame* parent, std::string name)
		: Frame(parent, name)
	{
		m_Numbers = new GUI::NumberFrame(this, "FPSCounterNumberFrame");
	}

	virtual void Update(double dt)
	{
		m_NumFrames++;
		m_Accum += dt;

		m_Numbers->SetNumber(1.0/(m_Accum/m_NumFrames));
		if (m_NumFrames % 20) {
			//LOG_INFO("FPS: %f", 1.0/(m_Accum/m_NumFrames));
		}
	}

private:
	NumberFrame* m_Numbers = nullptr;

	double m_Accum = 0.0;
	int m_NumFrames = 0;
};

}
}
#endif

