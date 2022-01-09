#pragma once

#include "algawolf.hpp"

extern Model *modelPolyVolt;

struct PolyVolt : WolfModule
{
	enum ParamIds
	{
		ENUMS(K_PARAM, 16),
		NUM_PARAMS
	};
	enum InputIds
	{
		NUM_INPUTS
	};
	enum OutputIds
	{
		OUT_015_OUTPUT,
		OUT_815_OUTPUT,
		OUT_07_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		NUM_LIGHTS
	};

	int m_16 = 16;

	dsp::ClockDivider m_divider;
	const uint32_t m_divisionLevels[5] = {1, 16, 64, 512, 44100};

	PolyVolt()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < 16; i++)
		{
			configParam(K_PARAM + i, -10.f, 10.f, 0.f, "voltage", " volts");
		}
		m_divider.setDivision(16);
	}

	void process(const ProcessArgs &args) override;
};

struct PolyVoltWidget : WolfModuleWidget
{
	PolyVoltWidget(PolyVolt *module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PolyVolt.svg")));
		for (int i = 0; i < 8; i++)
		{
			addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(5.092f, 5.291f + (i * 12.118f))), module, PolyVolt::K_PARAM + i));
			addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(5.092f, 5.291f + (i * 12.118f)))));

			addParam(createParamCentered<BlueKnob6>(mm2px(Vec(10.451f, 11.35f + (i * 12.118f))), module, PolyVolt::K_PARAM + i + 8));
			addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(10.451f, 11.35f + (i * 12.118f)))));
		}

		addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(5.092f, 105.634f)), module, PolyVolt::OUT_07_OUTPUT));
		addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(10.251f, 113.634f)), module, PolyVolt::OUT_815_OUTPUT));
		addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(7.42f, 122.634f)), module, PolyVolt::OUT_015_OUTPUT));
	}

	void appendContextMenu(Menu *menu) override
	{
		PolyVolt *module = dynamic_cast<PolyVolt *>(this->module);

		menu->addChild(new MenuEntry);
		menu->addChild(createMenuLabel("Update frequency (samples)"));

		for (int i = 0; i < 5; i++)
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
};
