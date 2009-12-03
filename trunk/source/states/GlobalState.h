// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef GLOBALSTATE_H_
#define GLOBALSTATE_H_

#include "../core/statemanager/State.h"

#include <irrTypes.h>

namespace irr
{
	class SEvent;
}

class Demo;

class GlobalState : public State<Demo, irr::SEvent>
{

public:

	GlobalState();

	void onEnter(Demo* const demo);

	void onLeave(Demo* const demo);

	void onUpdate(Demo* const demo);

	const bool onEvent(Demo* const demo, const irr::SEvent& event);

private:

	irr::s32 lastFPS;
};

#endif /*GLOBALSTATE_H_*/
