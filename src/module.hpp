#pragma once

#include <rack.hpp>
using namespace rack;
using namespace simd;

namespace algaw0lf
{

	struct W0lfModule : Module
	{
		// RefreshCounter refresh;
		int m_channelCount = 8;
		void setChannels();
	};

	struct W0lfModuleWidget : ModuleWidget
	{
	};

	// types

	struct alignas(16) CachedOutput
	{
		float_4 _voltages[4] = {float_4(0.f)};
		int _channelCount;

		CachedOutput(int channelCount)
		{
			this->_channelCount = channelCount;
		}

		void cacheVoltage(float voltage, int channel)
		{
			int ind = std::floor(channel / 4.f);
			_voltages[ind][channel % 4] = voltage;
		}

		void cache_10volts(int channel)
		{
			int ind = std::floor(channel / 4.f);
			_voltages[ind][channel % 4] = 10.f;
		}

		void cache_0volts(int channel)
		{
			int ind = std::floor(channel / 4.f);
			_voltages[ind][channel % 4] = 0.f;
		}

		void all_to_0volts()
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					_voltages[i][j] = 0.f;
				}
			}
		}

		void setCachedVoltagesToOutput(Output *output)
		{
			output->setChannels(_channelCount);
			int ind = 0;
			for (int i = 0; i < _channelCount; i += 4)
			{
				output->setVoltageSimd(_voltages[ind++], i);
			}
		}
	};

	// components

	struct alignas(16) CVParam
	{
		Param *param;
		Param *av;
		Input *cv;
		float min;
		float max;
		float range;

		void setCVParam(Param *param, Param *av, Input *cv, float min, float max)
		{
			this->param = param;
			this->av = av;
			this->cv = cv;
			this->min = min;
			this->max = max;
			this->range = std::abs(max - min) * 0.1f;
		}

		float getValue()
		{
			float x = param->getValue() + ((av->getValue() * cv->getVoltage() * range));
			return std::fmax(std::fmin(x, max), min);
		}
	};

	struct alignas(16) Note
	{
		float voltage;
		float delayOffset;
		float cv;
		int index;
		bool muted;
	};

	struct alignas(16) NoteGate
	{
		Note *note;
		float voltage = 0.f;
		double open = 0.f;
		double close = 0.f;
		float timeToNextNote = 0.f;
		bool isOpen = false;
	};

	struct RefreshCounter
	{
		// Note: because of stagger, and asyncronous dataFromJson, should not assume this processInputs() will return true on first run
		// of module::process()
		static const unsigned int displayRefreshStepSkips = 256;
		static const unsigned int userInputsStepSkipMask = 0xF; // sub interval of displayRefreshStepSkips, since inputs should be more responsive than lights
		// above value should make it such that inputs are sampled > 1kHz so as to not miss 1ms triggers

		unsigned int refreshCounter = (random::u32() % displayRefreshStepSkips); // stagger start values to avoid processing peaks when many Geo and Impromptu modules in the patch

		bool processInputs()
		{
			return ((refreshCounter & userInputsStepSkipMask) == 0);
		}

		bool processLights()
		{ // this must be called even if module has no lights, since counter is decremented here
			refreshCounter++;
			if (refreshCounter >= displayRefreshStepSkips)
			{
				refreshCounter = 0;
				return true;
			}

			return false;
		}
	};
} // namespace w0lf