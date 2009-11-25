// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef RUNNINGSTATE_H_
#define RUNNINGSTATE_H_

#include <vector2d.h>
#include <Keycodes.h>

#include "../core/statemanager/State.h"

namespace irr
{
	class SEvent;

	namespace gui
	{
		class IGUIStaticText;
		class Compass;
	}
}

class Demo;
class Map;

class RunningState : public State<Demo, irr::SEvent>
{

public:

	RunningState();

	~RunningState();

	void onEnter(Demo* const demo);

	void onLeave(Demo* const demo);

	const bool onEvent(Demo* const demo, const irr::SEvent& event);

	void onUpdate(Demo* const demo);

	void drawCallback(Demo* const demo) const;

private:

	Map* map;

	irr::gui::Compass* pgCompass;

	irr::gui::IGUIStaticText* infoText;

	///Array of keys pressed.
	bool keys[irr::KEY_KEY_CODES_COUNT];

	///Current mouse position.
	irr::core::vector2di mousePos;

	///State of right mouse button.
	bool rightMouseButton;

	bool freezeTarget;
};

#endif /*RUNNINGSTATE_H_*/
