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
		m_FrameTimes.resize(m_MaxSamples);
		m_Numbers = new GUI::NumberFrame(this, "FPSCounterNumberFrame");
	}

	virtual void Update(double dt)
	{
		m_FrameTimes[m_CurrentFrame % m_MaxSamples] = dt;
		double sum = std::accumulate(m_FrameTimes.begin(), m_FrameTimes.end(), 0.0);
		sum /= std::min(m_CurrentFrame, m_MaxSamples);
		m_Numbers->SetNumber(std::round(1.0 / sum));

		m_CurrentFrame++;
	}

private:
	NumberFrame* m_Numbers = nullptr;

	int m_MaxSamples = 100;
	int m_CurrentFrame = 0;
	std::vector<double> m_FrameTimes;
};

}
}
#endif

