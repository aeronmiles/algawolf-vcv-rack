#include "algaw0lf.hpp"

using namespace simd;

const float MIN_TIME = 1e-3f;
const float MAX_TIME = 10.f;
const float LAMBDA_BASE = MAX_TIME / MIN_TIME;

struct PolyADSR : Module
{
	enum ParamIds
	{
		ATTACK_PARAM,
		ATTACK_AV_PARAM,
		DECAY_PARAM,
		DECAY_AV_PARAM,
		SUSTAIN_PARAM,
		SUSTAIN_AV_PARAM,
		RELEASE_PARAM,
		RELEASE_AV_PARAM,
		NUM_PARAMS
	};
	enum InputIds
	{
		GATE_INPUT,
		ATTACK_CV_INPUT,
		DECAY_CV_INPUT,
		SUSTAIN_CV_INPUT,
		RELEASE_CV_INPUT,
		TRIG_INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
		ENVELOPE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		ATTACK_LIGHT,
		DECAY_LIGHT,
		SUSTAIN_LIGHT,
		RELEASE_LIGHT,
		NUM_LIGHTS
	};

	float_4 attacking[4] = {float_4::zero()};
	float_4 env[4] = {0.f};
	dsp::TSchmittTrigger<float_4> trigger[4];
	dsp::ClockDivider cvDivider;
	float_4 attackLambda[4] = {0.f};
	float_4 decayLambda[4] = {0.f};
	float_4 releaseLambda[4] = {0.f};
	float_4 sustain[4] = {0.f};
	dsp::ClockDivider lightDivider;

	PolyADSR()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ATTACK_PARAM, 0.f, 1.f, 0.5f, "Attack", " ms", LAMBDA_BASE, MIN_TIME * 1000);
		configParam(DECAY_PARAM, 0.f, 1.f, 0.5f, "Decay", " ms", LAMBDA_BASE, MIN_TIME * 1000);
		configParam(SUSTAIN_PARAM, 0.f, 1.f, 0.5f, "Sustain", "%", 0, 100);
		configParam(RELEASE_PARAM, 0.f, 1.f, 0.5f, "Release", " ms", LAMBDA_BASE, MIN_TIME * 1000);

		configParam(ATTACK_AV_PARAM, -1.f, 1.f, 0.f, "Attack CV Attenuverter");
		configParam(DECAY_AV_PARAM, -1.f, 1.f, 0.f, "Decay CV Attenuverter");
		configParam(SUSTAIN_AV_PARAM, -1.f, 1.f, 0.f, "Sustain CV Attenuverte");
		configParam(RELEASE_AV_PARAM, -1.f, 1.f, 0.f, "Release CV Attenuverter");

		cvDivider.setDivision(16);
		lightDivider.setDivision(128);
	}

	void process(const ProcessArgs &args) override
	{
		// 0.16-0.19 us serial
		// 0.23 us serial with all lambdas computed
		// 0.15-0.18 us serial with all lambdas computed with SSE

		int channels = inputs[GATE_INPUT].getChannels();

		// Compute lambdas
		if (cvDivider.process())
		{
			float attackParam = params[ATTACK_PARAM].getValue();
			float decayParam = params[DECAY_PARAM].getValue();
			float sustainParam = params[SUSTAIN_PARAM].getValue();
			float releaseParam = params[RELEASE_PARAM].getValue();
			float attackAV = params[ATTACK_AV_PARAM].getValue();
			float decayAV = params[DECAY_AV_PARAM].getValue();
			float sustainAV = params[SUSTAIN_AV_PARAM].getValue();
			float releaseAV = params[RELEASE_AV_PARAM].getValue();

			for (int c = 0; c < channels; c += 4)
			{
				// CV
				float_4 attack = attackParam + (inputs[ATTACK_CV_INPUT].getPolyVoltageSimd<float_4>(c) * attackAV) / 10.f;
				float_4 decay = decayParam + (inputs[DECAY_CV_INPUT].getPolyVoltageSimd<float_4>(c) * decayAV) / 10.f;
				float_4 sustain = sustainParam + (inputs[SUSTAIN_CV_INPUT].getPolyVoltageSimd<float_4>(c) * sustainAV) / 10.f;
				float_4 release = releaseParam + (inputs[RELEASE_CV_INPUT].getPolyVoltageSimd<float_4>(c) * releaseAV) / 10.f;

				attack = simd::clamp(attack, 0.f, 1.f);
				decay = simd::clamp(decay, 0.f, 1.f);
				sustain = simd::clamp(sustain, 0.f, 1.f);
				release = simd::clamp(release, 0.f, 1.f);

				attackLambda[c / 4] = simd::pow(LAMBDA_BASE, -attack) / MIN_TIME;
				decayLambda[c / 4] = simd::pow(LAMBDA_BASE, -decay) / MIN_TIME;
				releaseLambda[c / 4] = simd::pow(LAMBDA_BASE, -release) / MIN_TIME;
				this->sustain[c / 4] = sustain;
			}
		}

		float_4 gate[4];

		for (int c = 0; c < channels; c += 4)
		{
			// Gate
			gate[c / 4] = inputs[GATE_INPUT].getVoltageSimd<float_4>(c) >= 1.f;

			// Retrigger
			float_4 triggered = trigger[c / 4].process(inputs[TRIG_INPUT].getPolyVoltageSimd<float_4>(c));
			attacking[c / 4] = simd::ifelse(triggered, float_4::mask(), attacking[c / 4]);

			// Get target and lambda for exponential decay
			const float attackTarget = 1.2f;
			float_4 target = simd::ifelse(gate[c / 4], simd::ifelse(attacking[c / 4], attackTarget, sustain[c / 4]), 0.f);
			float_4 lambda = simd::ifelse(gate[c / 4], simd::ifelse(attacking[c / 4], attackLambda[c / 4], decayLambda[c / 4]), releaseLambda[c / 4]);

			// Adjust env
			env[c / 4] += (target - env[c / 4]) * lambda * args.sampleTime;

			// Turn off attacking state if envelope is HIGH
			attacking[c / 4] = simd::ifelse(env[c / 4] >= 1.f, float_4::zero(), attacking[c / 4]);

			// Turn on attacking state if gate is LOW
			attacking[c / 4] = simd::ifelse(gate[c / 4], attacking[c / 4], float_4::mask());

			// Set output
			outputs[ENVELOPE_OUTPUT].setVoltageSimd(10.f * env[c / 4], c);
		}

		outputs[ENVELOPE_OUTPUT].setChannels(channels);

		// Lights
		if (lightDivider.process())
		{
			lights[ATTACK_LIGHT].setBrightness(0);
			lights[DECAY_LIGHT].setBrightness(0);
			lights[SUSTAIN_LIGHT].setBrightness(0);
			lights[RELEASE_LIGHT].setBrightness(0);

			for (int c = 0; c < channels; c += 4)
			{
				const float epsilon = 0.01f;
				float_4 sustaining = (sustain[c / 4] <= env[c / 4]) & (env[c / 4] < sustain[c / 4] + epsilon);
				float_4 resting = (env[c / 4] < epsilon);

				if (simd::movemask(gate[c / 4] & attacking[c / 4]))
					lights[ATTACK_LIGHT].setBrightness(1);
				if (simd::movemask(gate[c / 4] & ~attacking[c / 4] & ~sustaining))
					lights[DECAY_LIGHT].setBrightness(1);
				if (simd::movemask(gate[c / 4] & ~attacking[c / 4] & sustaining))
					lights[SUSTAIN_LIGHT].setBrightness(1);
				if (simd::movemask(~gate[c / 4] & ~resting))
					lights[RELEASE_LIGHT].setBrightness(1);
			}
		}
	}
};

struct ADSRWidget : ModuleWidget
{
	ADSRWidget(PolyADSR *module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ADSR.svg")));

		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(17.436f, 12.251f)), module, PolyADSR::ATTACK_PARAM));
		addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(17.436f, 12.251f))));
		addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(6.482f, 16.265f)), module, PolyADSR::ATTACK_AV_PARAM));
		addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(6.482f, 16.265f))));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(17.436f, 33.867f)), module, PolyADSR::DECAY_PARAM));
		addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(17.436f, 33.867f))));
		addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(6.482f, 38.049f)), module, PolyADSR::DECAY_AV_PARAM));
		addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(6.482f, 38.049f))));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(17.436f, 55.484f)), module, PolyADSR::SUSTAIN_PARAM));
		addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(17.436f, 55.484f))));
		addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(6.482f, 59.833f)), module, PolyADSR::SUSTAIN_AV_PARAM));
		addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(6.482f, 59.833f))));
		addParam(createParamCentered<WhiteKnob10>(mm2px(Vec(17.436f, 77.1f)), module, PolyADSR::RELEASE_PARAM));
		addChild(createWidgetCentered<Knob10Highlights>(mm2px(Vec(17.436f, 77.1f))));
		addParam(createParamCentered<WhiteKnob6>(mm2px(Vec(6.482f, 81.617f)), module, PolyADSR::RELEASE_AV_PARAM));
		addChild(createWidgetCentered<Knob6Highlights>(mm2px(Vec(6.482f, 81.617f))));

		addInput(createInputCentered<PJ301MPortMetalWhite>(mm2px(Vec(7.156f, 96.462f)), module, PolyADSR::GATE_INPUT));
		addInput(createInputCentered<PJ301MPortMetalWhite>(mm2px(Vec(18.293f, 96.462f)), module, PolyADSR::TRIG_INPUT));
		addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(6.482f, 7.878f)), module, PolyADSR::ATTACK_CV_INPUT));
		addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(6.482f, 29.662f)), module, PolyADSR::DECAY_CV_INPUT));
		addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(6.482f, 51.446f)), module, PolyADSR::SUSTAIN_CV_INPUT));
		addInput(createInputCentered<PJ301MPortMetalCV>(mm2px(Vec(6.482f, 73.23f)), module, PolyADSR::RELEASE_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPortMetal>(mm2px(Vec(12.7f, 115.819f)), module, PolyADSR::ENVELOPE_OUTPUT));

		addChild(createLightCentered<MediumLight<W0lfRedLight>>(mm2px(Vec(17.436f, 3.768f)), module, PolyADSR::ATTACK_LIGHT));
		addChild(createLightCentered<MediumLight<W0lfRedLight>>(mm2px(Vec(17.436f, 25.42f)), module, PolyADSR::DECAY_LIGHT));
		addChild(createLightCentered<MediumLight<W0lfRedLight>>(mm2px(Vec(17.436f, 47.072f)), module, PolyADSR::SUSTAIN_LIGHT));
		addChild(createLightCentered<MediumLight<W0lfRedLight>>(mm2px(Vec(17.436f, 68.723f)), module, PolyADSR::RELEASE_LIGHT));
	}
};

Model *modelPolyADSR = createModel<PolyADSR, ADSRWidget>("PolyADSR");