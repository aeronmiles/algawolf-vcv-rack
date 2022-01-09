#pragma once

#include "algawolf.hpp"

extern Model *modelStrum;

struct Strum : WolfModule
{
    enum ParamIds
    {
        PATTERN_PARAM,
        PATTERN_AV_PARAM,
        DELAY_PARAM,
        DELAY_AV_PARAM,
        REPEAT_PARAM,
        REPEAT_AV_PARAM,
        GATE_PARAM,
        GATE_AV_PARAM,
        SELECT_CHORD_PARAM,
        RAND_NOTE_OFFSET_PARAM,
        RAND_NOTE_X_PARAM,
        ENUMS(DELAYOFFSET_PARAM, 8),
        ENUMS(X_PARAM, 8),
        ENUMS(NOTE_PARAM, 8),
        ENUMS(MUTE_PARAM, 8),
        // RAND_PARAM_PARAM,
        NUM_PARAMS
    };
    enum InputIds
    {
        OFFSET_CV_INPUT,
        X_CV_INPUT,
        GATE_INPUT,
        VOCT_INPUT,
        SEQ_CV_INPUT,
        VNOTE_CV_INPUT,
        REPEAT_CV_INPUT,
        PATTERN_CV_INPUT,
        DELAY_CV_INPUT,
        GATE_CV_INPUT,
        NUM_INPUTS
    };
    enum OutputIds
    {
        VOCT_OUTPUT,
        GATE_OUTPUT,
        SLIDE_OUTPUT,
        X_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        ENUMS(LED_LIGHT, 8 * 2), // GreenRed lights
        RPARAM_LIGHT,
        RNOTE_LIGHT,
        RSHIFT_LIGHT,
        RX_LIGHT,
        NUM_LIGHTS
    };

    // note settings
    static const int MAX_NOTES = 8;
    alignas(16) Note m_notes[8];
    int m_noteCount = 0;
    // note setting state
    int m_settingGateVoltage = 20.f;
    float m_sampleTime = 0.f;
    float m_gateVoltage = 0.f;
    float m_previousGateVoltage = 0.f;
    bool m_addNotes = false;

    // sequence params
    CVParam m_pattern;
    CVParam m_delay;
    CVParam m_repeat;
    CVParam m_gate;
    // sequence settings
    static const int MAX_SEQ_NOTES = 96;
    alignas(16) NoteGate m_gates[MAX_SEQ_NOTES];
    int m_gateStart = 0;
    int m_gateEnd = 0;
    float m_rootVoltageOffset = 0.f;
    // sequence state
    bool m_seqRunning = false;
    double m_time = 0.f;

    // input state
    wdsp::RiseTrigger t_mutes[8];
    wdsp::RiseTrigger t_gateIn;
    float m_chordIndex = 0.f;
    scales::Scale m_scale = scales::CHROMATIC;
    // intervals [i / 24, (i+1) / 24) V mapping to the closest enabled note
    int m_quantizeIntervals[24];
    float m_addNoteInterval = 1.f;

    // module state
    dsp::ClockDivider m_inputDivider;
    dsp::ClockDivider m_lightDivider;
    bool m_alternate = true;
    bool m_noteMuteOverwrite = true;
    bool m_mutesSetLength = false;
    bool m_quantize = true;

    void process(const ProcessArgs &args) override;
    void addGatedNotes();
    void addUngatedNotes();
    void addSeqNotes();
    void playSeq();
    void cvAddSeqNotes();
    void cvPlaySeq();
    void setParams();
    void checkMutes();
    void mute(bool _mute);
    void resetGates();
    void setChannels();
    void updateQuantizerNotes();
    void quantize();

    Strum()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(PATTERN_PARAM, 1.f, 22.f, 8.f, "Sequence Pattern", " notes");
        configParam(DELAY_AV_PARAM, 0.f, 1.f, 0.0833f, "Delay", " ms", 0.f, 1000.f);
        configParam(REPEAT_PARAM, -4.f, 4.f, 1.f, "Repeat Cycles", " cycles");
        configParam(GATE_PARAM, 0.01f, 2.f, 0.1f, "Gate Length", " ms", 0.f, 1000.f);

        configParam(PATTERN_AV_PARAM, -1.f, 1.f, 0.f, "Pattern CV Attenuverter");
        configParam(DELAY_AV_PARAM, -1.f, 1.f, 0.f, "Delay CV Attenuverter");
        configParam(REPEAT_AV_PARAM, -1.f, 1.f, 0.f, "Repeat CV Attenuverter");
        configParam(GATE_AV_PARAM, -1.f, 1.f, 0.f, "Gate CV Attenuverter");

        // configParam(RAND_PARAM_PARAM, 0.f, 1.f, 0.f, "Randomize Parameters");
        configParam(SELECT_CHORD_PARAM, 0.f, chords::NUMBER_CHORDS - 1, 0.f, "Chords", "");
        configParam(RAND_NOTE_OFFSET_PARAM, 0.f, 1.f, 0.f, "Randomize Offset");
        configParam(RAND_NOTE_X_PARAM, 0.f, 1.f, 0.f, "Randomize X Values");

        for (int i = 0; i < MAX_NOTES; i++)
        {
            configParam(NOTE_PARAM + i, -5.f, 5.f, 0.f, i == 0 ? "Root Note " : "Note " + std::to_string(i), " volts");
            configParam(DELAYOFFSET_PARAM + i, -1.f, 1.f, 0.f, "Note Offset " + std::to_string(i), "");
            configParam(X_PARAM + i, 0.f, 10.f, 5.f, "X" + std::to_string(i), " volts");
            configParam(MUTE_PARAM + i, 0.0f, 1.0f, 0.0f, "Mute");
        }

        m_inputDivider.setDivision(32);
        m_lightDivider.setDivision(64);

        m_pattern.setCVParam(&params[PATTERN_PARAM], &params[PATTERN_AV_PARAM], &inputs[PATTERN_CV_INPUT], 0.f, 22.f);
        m_delay.setCVParam(&params[DELAY_PARAM], &params[DELAY_AV_PARAM], &inputs[DELAY_CV_INPUT], 0.f, 1.f);
        m_repeat.setCVParam(&params[REPEAT_PARAM], &params[REPEAT_AV_PARAM], &inputs[REPEAT_CV_INPUT], -4.f, 4.f);
        m_gate.setCVParam(&params[GATE_PARAM], &params[GATE_AV_PARAM], &inputs[GATE_CV_INPUT], .01f, 2.f);

        for (int i = 0; i < MAX_NOTES; i++)
        {
            m_notes[i].index = i;
            m_notes[i].muted = false;
        }

        for (int i = 0; i < MAX_SEQ_NOTES; i++)
        {
            m_gates[i].note = &m_notes[i % MAX_NOTES];
        }

        // initialize
        setParams();
        setChannels();
    }

    void onReset() override
    {
        mute(false);
    }

    json_t *dataToJson() override
    {
        // settings
        json_t *rootJ = json_object();
        json_object_set_new(rootJ, "noteCount", json_integer(m_noteCount));
        json_object_set_new(rootJ, "addNoteInterval", json_real(m_addNoteInterval));
        json_object_set_new(rootJ, "noteMuteOverwrite", json_boolean(m_noteMuteOverwrite));
        json_object_set_new(rootJ, "mutesSetLength", json_boolean(m_mutesSetLength));
        json_object_set_new(rootJ, "quantize", json_boolean(m_quantize));

        // scale notes
        json_t *scaleJ = json_array();
        for (int i = 0; i < 12; i++)
        {
            json_array_insert_new(scaleJ, i, json_integer((int)m_scale.notes[i]));
        }
        json_object_set_new(rootJ, "scale", scaleJ);

        // mutes
        json_t *mutedJ = json_array();
        for (int i = 0; i < MAX_NOTES; i++)
        {
            json_array_insert_new(mutedJ, i, json_integer((int)m_notes[i].muted));
        }
        json_object_set_new(rootJ, "muted", mutedJ);

        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override
    {
        // settings
        json_t *noteCountJ = json_object_get(rootJ, "noteCount");
        if (noteCountJ)
            m_noteCount = json_integer_value(noteCountJ);

        json_t *noteIntervalJ = json_object_get(rootJ, "addNoteInterval");
        if (noteIntervalJ)
            m_addNoteInterval = json_number_value(noteIntervalJ);

        json_t *noteMuteOverwriteJ = json_object_get(rootJ, "noteMuteOverwrite");
        if (noteMuteOverwriteJ)
            m_noteMuteOverwrite = json_boolean_value(noteMuteOverwriteJ);

        json_t *mutesSetLengthJ = json_object_get(rootJ, "mutesSetLength");
        if (mutesSetLengthJ)
            m_mutesSetLength = json_boolean_value(mutesSetLengthJ);

        json_t *quantizeJ = json_object_get(rootJ, "quantize");
        if (quantizeJ)
            m_quantize = json_boolean_value(quantizeJ);

        // scale
        json_t *scaleJ = json_object_get(rootJ, "scale");
        if (scaleJ)
        {
            for (int i = 0; i < 12; i++)
            {
                json_t *sJ = json_array_get(scaleJ, i);
                if (sJ)
                    m_scale.notes[i] = !!json_integer_value(sJ);
            }
        }
        updateQuantizerNotes();
        quantize();

        // mutes
        json_t *mutedJ = json_object_get(rootJ, "muted");
        if (mutedJ)
        {
            for (int i = 0; i < MAX_NOTES; i++)
            {
                json_t *muteJ = json_array_get(mutedJ, i);
                if (muteJ)
                    m_notes[i].muted = !!json_integer_value(muteJ);
            }
        }
    }

    inline void onSetChord()
    {
        m_chordIndex = params[SELECT_CHORD_PARAM].getValue();
        chords::Chord chord = chords::CHORDS[(int)m_chordIndex];
        int n = 0;
        float octave = 0.f;
        float semiTone = 1.f / 12.f;

        if (m_noteMuteOverwrite)
            mute(true);

        while (n < 8)
        {
            for (int i = 0; i < 12; i++)
            {
                if (chord.notes[i])
                {
                    m_notes[n].voltage = octave + (i * semiTone);
                    params[NOTE_PARAM + n].setValue(m_notes[n].voltage);

                    if (octave == 0.f && m_noteMuteOverwrite)
                        m_notes[n].muted = false;

                    n++;
                }
                if (n >= 8)
                    break;
            }
            octave += 1.f;
        }
    }

    inline void onRandomizeOffsets()
    {
        float randOffset = params[RAND_NOTE_OFFSET_PARAM].getValue();
        if (randOffset < 0.001f)
        {
            for (int i = 0; i < MAX_NOTES; i++)
                params[DELAYOFFSET_PARAM + i].setValue(0.f);
        }
        else
        {
            srand(static_cast<int>(randOffset * INT_MAX));
            for (int i = 0; i < MAX_NOTES; i++)
                params[DELAYOFFSET_PARAM + i].setValue((static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 2)) * randOffset - (1.f * randOffset));
        }
    }

    inline void onRandomizeXs()
    {
        float randX = params[RAND_NOTE_X_PARAM].getValue();
        if (randX < 0.001f)
        {
            for (int i = 0; i < MAX_NOTES; i++)
                params[X_PARAM + i].setValue(5.f);
        }
        else
        {
            srand(static_cast<unsigned int>(randX * INT_MAX));
            for (int i = 0; i < MAX_NOTES; i++)
            {
                params[X_PARAM + i].setValue((static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10)) * randX + ((1.f - randX) * 5.f));
            }
        }
    }

    // inline void onRandomizeMutes()
    // {
    //     m_randMutes = params[RAND_NOTE_X_PARAM].getValue();
    //     if (m_randMutes < 0.001f)
    //     {
    //         for (int i = 0; i < MAX_NOTES; i++)
    //         {
    //             params[MUTE_PARAM + i].setValue(1.f);
    //             lights[LED_LIGHT + (i << 1)].setBrightness(1.f);
    //             m_notes[i].muted = false;
    //         }
    //     }
    //     else
    //     {
    //         srand(static_cast<unsigned int>(m_randMutes * 1000.f));
    //         float x;
    //         for (int i = 0; i < MAX_NOTES; i++)
    //         {
    //             x = random::uniform() > 0.3f;
    //             m_notes[i].muted = !x;
    //             params[MUTE_PARAM + i].setValue(x);
    //             lights[LED_LIGHT + (i << 1)].setBrightness(x);
    //         }
    //     }
    // }

    inline void onRandomizeParams()
    {
        params[PATTERN_PARAM].setValue(random::u32() % 0x17);
        params[REPEAT_AV_PARAM].setValue(-4.f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 8)));
        params[DELAY_PARAM].setValue(static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)));
        params[GATE_PARAM].setValue(static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)));
    }

    inline void resetLeds()
    {
        float sampleTime = m_sampleTime * m_lightDivider.division;
        for (int i = 0; i < MAX_NOTES; i++)
        {
            lights[LED_LIGHT + (i << 1)].setSmoothBrightness(m_notes[i].muted ? 0.f : 1.f, sampleTime);
            lights[LED_LIGHT + (i << 1) + 1].setSmoothBrightness(0.f, sampleTime);
        }
    }
};

struct NoteIntervalsItem : MenuItem
{
    ModuleWidget *moduleWidget;
    Strum *module;

    NoteIntervalsItem(Strum *module, ModuleWidget *moduleWidget)
    {
        this->module = module;
        this->moduleWidget = moduleWidget;
        this->text = "Padding Note Intervals";
        this->rightText = RIGHT_ARROW;
    }

    Menu *createChildMenu() override
    {
        Menu *menu = new Menu;
        Strum *m = this->module;

        LambdaMenuItem *intItem = new LambdaMenuItem(
            "-36",
            [m]() { m->m_addNoteInterval = -3.f; },
            [m]() { return m->m_addNoteInterval == -3.f; });
        menu->addChild(intItem);

        intItem = new LambdaMenuItem(
            "-24",
            [m]() { m->m_addNoteInterval = -2.f; },
            [m]() { return m->m_addNoteInterval == -2.f; });
        menu->addChild(intItem);

        // -12 to 0
        for (int i = 12; i >= 0; i--)
        {
            intItem = new LambdaMenuItem(
                ("-" + std::to_string(static_cast<int>(i))).c_str(),
                [m, i]() { m->m_addNoteInterval = -i / 12.f; },
                [m, i]() { return m->m_addNoteInterval == -i / 12.f; });
            menu->addChild(intItem);
        }
        // 1 to +12
        for (int i = 1; i <= 12; i++)
        {
            intItem = new LambdaMenuItem(
                ("+" + std::to_string(static_cast<int>(i))).c_str(),
                [m, i]() { m->m_addNoteInterval = i / 12.f; },
                [m, i]() { return m->m_addNoteInterval == i / 12.f; });
            menu->addChild(intItem);
        }

        intItem = new LambdaMenuItem(
            "+24",
            [m]() { m->m_addNoteInterval = 2.f; },
            [m]() { return m->m_addNoteInterval == 2.f; });
        menu->addChild(intItem);

        intItem = new LambdaMenuItem(
            "+36",
            [m]() { m->m_addNoteInterval = 3.f; },
            [m]() { return m->m_addNoteInterval == 3.f; });
        menu->addChild(intItem);

        return menu;
    }
};

struct StrumWidget : WolfModuleWidget
{
    const float leftPadding = 1.35f;
    const float rightPadding = leftPadding * 3.f;

    Strum *module;
    StrumWidget(Strum *module)
    {
        this->module = module;
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/StrumBlue.svg")));

        addInputs();
        addOutputs();
        addLeds();
        addKnobs(module);
    }

    void appendContextMenu(Menu *menu) override
    {
        Strum *module = dynamic_cast<Strum *>(this->module);
        addSettings(menu, module);
        addScales(menu, module);
    }

    void addSettings(Menu *menu, Strum *module)
    {
        menu->addChild(new MenuSeparator);
        menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Settings"));

        LambdaMenuItem *noteMuteOverwrite = new LambdaMenuItem(
            "Inputs Overwrite Mutes",
            [module]() { module->m_noteMuteOverwrite = !module->m_noteMuteOverwrite; },
            [module]() { return module->m_noteMuteOverwrite; });
        menu->addChild(noteMuteOverwrite);

        LambdaMenuItem *noteLengthMode = new LambdaMenuItem(
            "Last Mute Sets End Note",
            [module]() { module->m_mutesSetLength = !module->m_mutesSetLength; },
            [module]() { return module->m_mutesSetLength; });
        menu->addChild(noteLengthMode);

        NoteIntervalsItem *noteIntervals = new NoteIntervalsItem(module, this);
        menu->addChild(noteIntervals);

        LambdaMenuItem *quantizeToggle = new LambdaMenuItem(
            "Quantize",
            [module]() { module->m_quantize = !module->m_quantize; },
            [module]() { return module->m_quantize; });
        menu->addChild(quantizeToggle);
    }

    void addScales(Menu *menu, Strum *module)
    {
        menu->addChild(new MenuSeparator);
        menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Quantize Scales"));
        LambdaMenuItem *chromatic = new LambdaMenuItem(
            "Chromatic",
            [module]() { module->m_scale = scales::CHROMATIC; module->updateQuantizerNotes(); },
            [module]() { return module->m_scale == scales::CHROMATIC; });
        menu->addChild(chromatic);

        LambdaMenuItem *major = new LambdaMenuItem(
            "Major",
            [module]() { module->m_scale = scales::MAJOR; module->updateQuantizerNotes(); },
            [module]() { return module->m_scale == scales::MAJOR; });
        menu->addChild(major);

        LambdaMenuItem *minor = new LambdaMenuItem(
            "Minor",
            [module]() { module->m_scale = scales::MINOR; module->updateQuantizerNotes(); },
            [module]() { return module->m_scale == scales::MINOR; });
        menu->addChild(minor);

        LambdaMenuItem *harmMinor = new LambdaMenuItem(
            "Harmonic Minor",
            [module]() { module->m_scale = scales::HARMONIC_MINOR; module->updateQuantizerNotes(); },
            [module]() { return module->m_scale == scales::HARMONIC_MINOR; });
        menu->addChild(harmMinor);

        LambdaMenuItem *blues = new LambdaMenuItem(
            "Blues",
            [module]() { module->m_scale = scales::BLUES; module->updateQuantizerNotes(); },
            [module]() { return module->m_scale == scales::BLUES; });
        menu->addChild(blues);

        LambdaMenuItem *pentatonic = new LambdaMenuItem(
            "Pentatonic",
            [module]() { module->m_scale = scales::PENTATONIC; module->updateQuantizerNotes(); },
            [module]() { return module->m_scale == scales::PENTATONIC; });
        menu->addChild(pentatonic);
    }

    void addKnobs(Strum *module)
    {
        // Param knobs
        auto w = createParamCentered<WhiteKnob10>(mm2px(Vec(leftPadding + 16.335f, 10.896f)), module, Strum::PATTERN_PARAM);
        dynamic_cast<Knob *>(w)->snap = true;
        addParam(w);
        addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(leftPadding + 16.335f, 10.896f))));

        addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(leftPadding + 16.335f, 28.686f)), module, Strum::REPEAT_PARAM));
        addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(leftPadding + 16.335f, 28.686f))));
        addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(leftPadding + 16.335f, 46.514f)), module, Strum::DELAY_PARAM));
        addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(leftPadding + 16.335f, 46.514f))));
        addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(leftPadding + 16.335f, 64.342f)), module, Strum::GATE_PARAM));
        addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(leftPadding + 16.335f, 64.342f))));

        addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(leftPadding + 5.445f, 15.748f)), module, Strum::PATTERN_AV_PARAM));
        addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(leftPadding + 5.445f, 33.906f)), module, Strum::REPEAT_AV_PARAM));
        addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(leftPadding + 5.445f, 52.065f)), module, Strum::DELAY_AV_PARAM));
        addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(leftPadding + 5.445f, 70.224f)), module, Strum::GATE_AV_PARAM));
        addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(leftPadding + 5.445f, 15.748f))));
        addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(leftPadding + 5.445f, 33.906f))));
        addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(leftPadding + 5.445f, 52.065f))));
        addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(leftPadding + 5.445f, 70.224f))));

        // Random knobs
        auto k = createParamCentered<KnobEvent<BlackKnob6>>(mm2px(Vec(leftPadding + 3.856f, 124.604f)), module, Strum::SELECT_CHORD_PARAM);
        ;
        dynamic_cast<Knob *>(k)->snap = true;
        k->runOnDragMove = [module]() { module->onSetChord(); };
        addParam(k);
        addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(leftPadding + 3.856f, 124.604f))));

        k = createParamCentered<KnobEvent<BlackKnob6>>(mm2px(Vec(leftPadding + 12.563f, 124.604f)), module, Strum::RAND_NOTE_OFFSET_PARAM);
        ;
        k->runOnDragMove = [module]() { module->onRandomizeOffsets(); };
        addParam(k);
        addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(leftPadding + 12.563f, 124.604f))));

        k = createParamCentered<KnobEvent<BlackKnob6>>(mm2px(Vec(leftPadding + 21.27f, 124.604f)), module, Strum::RAND_NOTE_X_PARAM);
        ;
        k->runOnDragMove = [module]() { module->onRandomizeXs(); };
        addParam(k);
        addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(leftPadding + 21.27f, 124.604f))));

        // Note knobs
        auto knote0 = createParamCentered<KnobEvent<RedKnob10>>(mm2px(Vec(rightPadding + 38.114f, 9.704f)), module, Strum::NOTE_PARAM);
        knote0->runOnDragEnd = [module]() { module->quantize(); };
        addParam(knote0);
        addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(rightPadding + 38.114f, 9.704f))));
        for (int i = 0; i < 8; i++)
        {
            float yoffset = i * 15;
            if (i != 0)
            {
                auto kNoteI = createParamCentered<KnobEvent<BlackKnob10>>(mm2px(Vec(rightPadding + 38.114f, 9.704f + yoffset)), module, Strum::NOTE_PARAM + i);
                kNoteI->runOnDragEnd = [module]() { module->quantize(); };
                addParam(kNoteI);
                addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(rightPadding + 38.114f, 9.704f + yoffset))));
            }

            addParam(createParamCentered<BlackKnob6>(mm2px(Vec(rightPadding + 30.164f, 3.169f + yoffset)), module, Strum::DELAYOFFSET_PARAM + i));
            addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(rightPadding + 30.164f, 3.169f + yoffset))));

            addParam(createParamCentered<BlackKnob6>(mm2px(Vec(rightPadding + 46.102f, 3.169f + yoffset)), module, Strum::X_PARAM + i));
            addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(rightPadding + 46.102f, 3.169f + yoffset))));
        }
    }

    void addInputs()
    {
        addInput(createInputCentered<PJ301MPortMetalBlue>(mm2px(Vec(leftPadding + 7.589f, 81.486f)), module, Strum::VOCT_INPUT));
        addInput(createInputCentered<PJ301MPortMetalBlue>(mm2px(Vec(leftPadding + 18.131f, 81.486f)), module, Strum::GATE_INPUT));
        
        addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(leftPadding + 5.445f, 43.678f)), module, Strum::DELAY_CV_INPUT));
        addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(leftPadding + 5.445f, 7.361f)), module, Strum::PATTERN_CV_INPUT));
        addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(leftPadding + 5.445f, 25.519f)), module, Strum::REPEAT_CV_INPUT));
        addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(leftPadding + 5.445f, 61.837f)), module, Strum::GATE_CV_INPUT));

        addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(rightPadding + 30.164f, 124.604f)), module, Strum::OFFSET_CV_INPUT));
        addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(rightPadding + 46.102f, 124.604f)), module, Strum::X_CV_INPUT));
        addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(rightPadding + 38.111f, 124.604f)), module, Strum::SEQ_CV_INPUT));
        
    }

    void addOutputs()
    {
        addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(leftPadding + 7.589f, 98.342f)), module, Strum::VOCT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(leftPadding + 18.131f, 98.342f)), module, Strum::GATE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(leftPadding + 7.589f, 111.439f)), module, Strum::SLIDE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(leftPadding + 18.131f, 111.439f)), module, Strum::X_OUTPUT));
    }

    void addLeds()
    {
        for (int i = 0; i < 8; i++)
        {
            addChild(createLightCentered<MediumLight<CyanRedLight>>(mm2px(Vec(rightPadding + 48.034f, 9.51f + (i * 15))), module, Strum::LED_LIGHT + i * 2));
            addParam(createParamCentered<TransparentButton>(mm2px(Vec(rightPadding + 48.034f, 9.51f + (i * 15))), module, Strum::MUTE_PARAM + i));
        }
    }
};