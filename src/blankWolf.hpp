#pragma once

#include "algawolf.hpp"

struct BlankWolf : Module
{
    BlankWolf()
    {
        config(0, 0, 0, 0);
    }
};

struct BlankWolfWidget : ModuleWidget
{
    BlankWolfWidget(BlankWolf *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BlankWolf.svg")));
    }
};

Model *modelBlankWolf = createModel<BlankWolf, BlankWolfWidget>("BlankWolf");
