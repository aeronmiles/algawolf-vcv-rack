#pragma once

#include "algaw0lf.hpp"

struct BlankW0lf : Module
{
    BlankW0lf()
    {
        config(0, 0, 0, 0);
    }
};

struct BlankW0lfWidget : ModuleWidget
{
    BlankW0lfWidget(BlankW0lf *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BlankW0lf.svg")));
    }
};

Model *modelBlankW0lf = createModel<BlankW0lf, BlankW0lfWidget>("BlankW0lf");
