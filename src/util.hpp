#pragma once

#include "rack.hpp"
#include <cfloat>

namespace algaw0lf::util
{

    inline float addCv(Param *knob, Param *attenuverter, Input *cv)
    {
        return knob->getValue() + (attenuverter->getValue() * cv->getVoltage());
    }

    inline float addCv(float value, Param *attenuverter, Input *cv)
    {
        return value + (attenuverter->getValue() * cv->getVoltage());
    }

    inline float addCv(Param *knob, Param *attenuverter, Input *cv, float min, float max)
    {
        return clamp(knob->getValue() + (attenuverter->getValue() * cv->getVoltage()), min, max);
    }

    inline float addCv(float value, Param *attenuverter, Input *cv, float min, float max)
    {
        return clamp(value + (attenuverter->getValue() * cv->getVoltage()), min, max);
    }

} // namespace w0lf::util
