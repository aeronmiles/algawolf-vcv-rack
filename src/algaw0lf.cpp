#include "algaw0lf.hpp"

// modules
#include "blankW0lf.hpp"
#include "orbit.hpp"
#include "polyAdsr.hpp"
#include "polyVolt.hpp"
#include "strum.hpp"

Plugin *pluginInstance;

void init(rack::Plugin *p)
{
	pluginInstance = p;

	p->addModel(modelOrbit);
	p->addModel(modelPolyADSR);
	p->addModel(modelPolyVolt);
	p->addModel(modelStrum);
	p->addModel(modelBlankW0lf);

}
