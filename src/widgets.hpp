#pragma once

#include <rack.hpp>
#include <functional>
#include "theme.hpp"

using namespace rack;

extern Plugin *pluginInstance;

namespace algaw0lf
{

	// BUTTONS

	struct TransparentButton : SvgSwitch
	{
		TransparentButton()
		{
			momentary = true;
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/TransparentButton.svg")));
			shadow->opacity = 0.f;
		}
	};

	// KNOBS

	template <typename TBase = RoundKnob>
	struct KnobEvent : TBase
	{
		std::function<void()> runOnDragMove = []() {};
		std::function<void()> runOnDragEnd = []() {};

		void onDragMove(const event::DragMove &e) override
		{
			Knob::onDragMove(e);
			runOnDragMove();
		}

		void onDragEnd(const event::DragEnd &e) override
		{
			Knob::onDragEnd(e);
			runOnDragEnd();
		}
	};

	struct BlackKnob10 : RoundKnob
	{
		BlackKnob10()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob10mmBlack.svg")));
		}
	};

	struct BlueKnob10 : RoundKnob
	{
		BlueKnob10()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob10mmBlue.svg")));
		}
	};

	struct RedKnob10 : RoundKnob
	{
		RedKnob10()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob10mmRed.svg")));
		}
	};

	struct BlackKnob6 : RoundKnob
	{
		BlackKnob6()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob6mmBlack.svg")));
		}
	};

	struct BlackKnob5 : RoundKnob
	{
		BlackKnob5()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob5mmBlack.svg")));
		}
	};

	struct WhiteKnob10 : RoundKnob
	{
		WhiteKnob10()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob10mmWhite.svg")));
		}
	};

	struct WhiteKnob6 : RoundKnob
	{
		WhiteKnob6()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob6mmWhite.svg")));
		}
	};

	struct BlueKnob6 : RoundKnob
	{
		BlueKnob6()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob6mmBlue.svg")));
		}
	};

	struct RedKnob6 : RoundKnob
	{
		RedKnob6()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob6mmRed.svg")));
		}
	};

	struct WhiteKnob5 : RoundKnob
	{
		WhiteKnob5()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob5mmWhite.svg")));
		}
	};

	struct Knob10Highlights : app::SvgScrew
	{
		Knob10Highlights()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob10mmHighlights.svg")));
		}
	};

	struct Knob6Highlights : app::SvgScrew
	{
		Knob6Highlights()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob6mmHighlights.svg")));
		}
	};

	struct Knob5Highlights : app::SvgScrew
	{
		Knob5Highlights()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Knob5mmHighlights.svg")));
		}
	};

	// PORTS

	struct PJ301MPortMetal : app::SvgPort
	{
		PJ301MPortMetal()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301M.svg")));
		}
	};

	struct PJ301MPortMetalGray : app::SvgPort
	{
		PJ301MPortMetalGray()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301MGray.svg")));
		}
	};

	struct PJ301MPortMetalWhite : app::SvgPort
	{
		PJ301MPortMetalWhite()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301MWhite.svg")));
		}
	};

	struct PJ301MPortMetalBlack : app::SvgPort
	{
		PJ301MPortMetalBlack()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301MBlack.svg")));
		}
	};

	struct PJ301MPortMetalRed : app::SvgPort
	{
		PJ301MPortMetalRed()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301MRed.svg")));
		}
	};

	struct PJ301MPortMetalBlue : app::SvgPort
	{
		PJ301MPortMetalBlue()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301MBlue.svg")));
		}
	};

	struct PJ301MPortMetalCV : app::SvgPort
	{
		PJ301MPortMetalCV()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301MCV.svg")));
			shadow->opacity = 0.f;
		}
	};

	struct PJ301MPortMetalCVRed : app::SvgPort
	{
		PJ301MPortMetalCVRed()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301MCVRed.svg")));
			shadow->opacity = 0.f;
		}
	};

	struct PJ301MPortMetalCVBlack : app::SvgPort
	{
		PJ301MPortMetalCVBlack()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/PJ301MCVBlack.svg")));
			shadow->opacity = 0.f;
		}
	};

	struct Button5Blue : app::SvgSwitch
	{
		Button5Blue()
		{
			momentary = true;
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Button5mmBlue.svg")));
		}
	};

	struct Button5Green : app::SvgSwitch
	{
		Button5Green()
		{
			momentary = true;
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Button5mmGreen.svg")));
		}
	};

	struct Button5White : app::SvgSwitch
	{
		Button5White()
		{
			momentary = true;
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Button5mmWhite.svg")));
		}
	};

	struct Button4White : app::SvgSwitch
	{
		Button4White()
		{
			momentary = true;
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Button4mmWhite.svg")));
		}
	};

	// LEDS

	template <typename TBase = GrayModuleLightWidget>
	struct TCyanRedLight : TBase
	{
		TCyanRedLight()
		{
			this->addBaseColor(theme::W0LF_CYAN);
			this->addBaseColor(theme::W0LF_RED);
		}
	};
	typedef TCyanRedLight<> CyanRedLight;

	template <typename TBase = GrayModuleLightWidget>
	struct TW0lfRedLight : TBase
	{
		TW0lfRedLight()
		{
			this->addBaseColor(theme::W0LF_RED);
		}
	};
	typedef TW0lfRedLight<> W0lfRedLight;

	// DIALS

	struct Dial10 : app::SvgScrew
	{
		Dial10()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/DialMarksWhite10mm.svg")));
		}
	};

	struct Dial10_Bi90Deg : app::SvgScrew
	{
		Dial10_Bi90Deg()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/DialMarksWhite10mmBi90deg.svg")));
		}
	};

	struct Dial10_01 : app::SvgScrew
	{
		Dial10_01()
		{
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/DialMarksWhite10mm01.svg")));
		}
	};

} // namespace w0lf