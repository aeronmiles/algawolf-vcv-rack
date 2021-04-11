#pragma once

namespace algaw0lf::wdsp
{
    struct RiseTrigger
    {
        bool state = false;

        void reset()
        {
            state = false;
        }

        bool process(float in)
        {
            if (state)
            {
                if (in <= 0.f)
                {
                    state = false;
                }
            }
            else
            {
                if (!state && in >= 1.f)
                {
                    state = true;
                    return true;
                }
            }
            return false;
        }
    };

    // https://www.desmos.com/calculator/wurcv7himo
    inline float fold_uni_10(float x)
    {
        return (std::signbit(x) ? -1.f : 1.f) * 20.f * std::fabs(x * 0.05f - std::round(x * 0.05f));
    }

    // https://www.youtube.com/watch?v=1xlCVBIF_ig&ab_channel=javidx9
    inline float fsin(float x)
    {
        float t = x * 0.15915f;
        t = t - (int)t;
        return 20.785f * t * (t - 0.5f) * (t - 1.f);
    }

    template <typename T>
    inline T sin2pi_fast(T x)
    {
        x -= 0.5f;
        return (T(-6.283185307) * x + T(33.19863968) * simd::pow(x, 3) - T(32.44191367) * simd::pow(x, 5)) / (1 + T(1.296008659) * simd::pow(x, 2) + T(0.7028072946) * simd::pow(x, 4));
    }

} // namespace w0lf::wdsp