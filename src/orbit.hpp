#pragma once

#include "algaw0lf.hpp"

extern Model *modelOrbit;

struct Orbit : W0lfModule
{
    enum ParamIds
    {
        ELEVATION_PARAM,
        ELEVATION_DECAY_PARAM,
        WIDTH_PARAM,
        WIDTH_DECAY_PARAM,
        NUM_PARAMS
    };
    enum InputIds
    {
        X_INPUT,
        Y_INPUT,
        NUM_INPUTS
    };
    enum OutputIds
    {
        ELEVATION_OUTPUT,
        ANGLE_OUTPUT,
        WIDTH_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    dsp::ClockDivider m_divider;
    const uint32_t m_divisionLevels[9] = {1, 8, 16, 32, 64, 128, 256, 512, 1024};
    // outputs
    float m_elevation;
    float m_angle;
    float m_width;

    Orbit()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(ELEVATION_PARAM, -5.f, 5.f, 5.f, "+|- Elevation at zero distance", "°", 0.f, 18.f);
        configParam(ELEVATION_DECAY_PARAM, 0.f, 10.f, 10.f, "Elevation decay to horizon", "", 0.f, 0.1f);
        configParam(WIDTH_PARAM, 0.f, 10.f, 5.f, "Width", "", 0.f, 0.1f);
        configParam(WIDTH_DECAY_PARAM, 0.f, 10.f, 10.f, "Width decay", "", 0.f, 0.1f);

        m_divider.setDivision(8);
    }

    void process(const ProcessArgs &args) override;
};

struct OrbitWidget : W0lfModuleWidget
{
    ParamWidget *knobs[4];
    Widget *highlights[4];
    int knobType = 0;

    OrbitWidget(Orbit *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Orbit.svg")));

        // create_dials();
        create_knobs();

        // Inputs : x | y
        addInput(createInputCentered<PJ301MPortMetalWhite>(mm2px(Vec(9.818f, 67.592f)), module, Orbit::X_INPUT));
        addInput(createInputCentered<PJ301MPortMetalWhite>(mm2px(Vec(5.818f, 77.47f)), module, Orbit::Y_INPUT));

        // Outputs : m_elevation | m_angle | m_width
        addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(7.62f, 88.957f)), module, Orbit::ELEVATION_OUTPUT));
        addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(7.62f, 102.456f)), module, Orbit::ANGLE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(7.62f, 115.822f)), module, Orbit::WIDTH_OUTPUT));
    }

    void appendContextMenu(Menu *menu) override
    {
        Orbit *module = dynamic_cast<Orbit *>(this->module);

        menu->addChild(new MenuEntry);
        menu->addChild(createMenuLabel("Update frequency (samples)"));

        for (int i = 0; i < 9; i++)
        {
            uint32_t div = module->m_divisionLevels[i];
            LambdaMenuItem *divItem = new LambdaMenuItem(
                std::to_string(module->m_divisionLevels[i]).c_str(),
                [module, div]() { module->m_divider.setDivision(div); },
                []() { return false; });
            divItem->rightText = CHECKMARK(module->m_divider.getDivision() == div);
            menu->addChild(divItem);
        }
    }

    void create_knobs()
    {
        if (knobType == 0)
        {
            create_black_knobs();
        }
        if (knobType == 1)
        {
            create_white_knobs();
        }
        create_highlights();
    }

    void remove_knobs()
    {
        for (int i = 0; i < 4; i++)
        {
            removeChild(knobs[i]);
            removeChild(highlights[i]);
        }
    }

    void create_dials()
    {
        // Knob dials
        addChild(createWidgetCentered<Dial10>(mm2px(Vec(7.539f, 7.942f))));
        addChild(createWidgetCentered<Dial10>(mm2px(Vec(7.539f, 22.911f))));
        addChild(createWidgetCentered<Dial10>(mm2px(Vec(7.539f, 37.88f))));
        addChild(createWidgetCentered<Dial10>(mm2px(Vec(7.539f, 52.849f))));
    }

    void create_highlights()
    {
        // Knob highlights
        highlights[0] = createWidgetCentered<Knob10Highlights>(mm2px(Vec(7.539f, 52.349f)));
        addChild(highlights[0]);
        highlights[1] = createWidgetCentered<Knob10Highlights>(mm2px(Vec(7.539f, 37.38f)));
        addChild(highlights[1]);
        highlights[2] = createWidgetCentered<Knob10Highlights>(mm2px(Vec(7.539f, 7.442f)));
        addChild(highlights[2]);
        highlights[3] = createWidgetCentered<Knob10Highlights>(mm2px(Vec(7.539f, 22.411f)));
        addChild(highlights[3]);
    }

    void create_black_knobs()
    {
        // E : m_elevation
        knobs[0] = createParamCentered<WhiteKnob10>(mm2px(Vec(7.539f, 7.442f)), module, Orbit::ELEVATION_PARAM);
        addParam(knobs[0]);

        // ED : m_elevation decay
        knobs[1] = createParamCentered<WhiteKnob10>(mm2px(Vec(7.539f, 22.411f)), module, Orbit::ELEVATION_DECAY_PARAM);
        addParam(knobs[1]);

        // W : m_width
        knobs[2] = createParamCentered<WhiteKnob10>(mm2px(Vec(7.539f, 37.38f)), module, Orbit::WIDTH_PARAM);
        addParam(knobs[2]);

        // WD : eidth decay
        knobs[3] = createParamCentered<WhiteKnob10>(mm2px(Vec(7.469f, 52.349f)), module, Orbit::WIDTH_DECAY_PARAM);
        addParam(knobs[3]);
    }

    void create_white_knobs()
    {
        // E : m_elevation
        knobs[0] = createParamCentered<WhiteKnob10>(mm2px(Vec(7.539f, 7.442f)), module, Orbit::ELEVATION_PARAM);
        addParam(knobs[0]);

        // ED : m_elevation decay
        knobs[1] = createParamCentered<WhiteKnob10>(mm2px(Vec(7.539f, 22.411f)), module, Orbit::ELEVATION_DECAY_PARAM);
        addParam(knobs[1]);

        // W : m_width
        knobs[2] = createParamCentered<WhiteKnob10>(mm2px(Vec(7.539f, 37.38f)), module, Orbit::WIDTH_PARAM);
        addParam(knobs[2]);

        // WD : eidth decay
        knobs[3] = createParamCentered<WhiteKnob10>(mm2px(Vec(7.469f, 52.349f)), module, Orbit::WIDTH_DECAY_PARAM);
        addParam(knobs[3]);
    }
};