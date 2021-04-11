#include "polyVolt.hpp"

void PolyVolt::process(const ProcessArgs &args)
{
    if (m_divider.process())
    {
        if (outputs[OUT_07_OUTPUT].isConnected())
        {
            for (int i = 0; i < 8; i++)
            {
                outputs[OUT_07_OUTPUT].setVoltage(params[K_PARAM + i].getValue(), i);
            }
            outputs[OUT_07_OUTPUT].setChannels(8);
        }

        if (outputs[OUT_815_OUTPUT].isConnected())
        {
            for (int i = 0; i < 8; i++)
            {
                outputs[OUT_815_OUTPUT].setVoltage(params[K_PARAM + i + 8].getValue(), i);
            }
            outputs[OUT_815_OUTPUT].setChannels(8);
        }

        if (outputs[OUT_015_OUTPUT].isConnected())
        {
            for (int i = 0; i < 8; i++)
            {
                outputs[OUT_015_OUTPUT].setVoltage(params[K_PARAM + i].getValue(), i);
                outputs[OUT_015_OUTPUT].setVoltage(params[K_PARAM + i + 8].getValue(), i + 8);
            }
            outputs[OUT_015_OUTPUT].setChannels(m_16);
        }
    }
}

Model *modelPolyVolt = createModel<PolyVolt, PolyVoltWidget>("PolyVolt");

