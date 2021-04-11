#include "orbit.hpp"

void Orbit::process(const ProcessArgs &args)
{
	if (!m_divider.process())
		return;

	float x = inputs[X_INPUT].getVoltage();
	float y = inputs[Y_INPUT].getVoltage();

	// normalize to 0 - 10 volts where 0v = -180 degrees, 5v = 0 degrees, 10v = 180 degree
	// normalize radians to +/- 5 volts @ 180 degrees = (5/pi) = 1.591549430
	m_angle = 5.f + (std::atan2(x, -y) * 1.591549430f);

	// max distance(5,5) = 7.0710678118654755 ^ 2 = 5^2 + 5^2 = 50.f
	float d = 1.f - (((x * x) + (y * y)) / 50.f);
	m_elevation = 5.f + (std::pow(d, params[ELEVATION_DECAY_PARAM].getValue()) * params[ELEVATION_PARAM].getValue());

	m_width = std::pow(d, params[WIDTH_DECAY_PARAM].getValue()) * params[WIDTH_PARAM].getValue();
	outputs[ELEVATION_OUTPUT].setVoltage(m_elevation);
	outputs[ANGLE_OUTPUT].setVoltage(m_angle);
	outputs[WIDTH_OUTPUT].setVoltage(m_width);
}

Model *modelOrbit = createModel<Orbit, OrbitWidget>("Orbit");
