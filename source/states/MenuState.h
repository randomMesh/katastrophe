// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef MENUSTATE_H_
#define MENUSTATE_H_

#include "../core/statemanager/State.h"

namespace irr
{
	class SEvent;

	namespace scene
	{
		class IMeshSceneNode;
		class ILightSceneNode;
	}

	namespace gui
	{
		class IGUIButton;
	}
}

class Demo;

class MenuState : public State<Demo, irr::SEvent>
{

public:

	MenuState();

	void onEnter(Demo* const demo);

	void onLeave(Demo* const demo);

	void onUpdate(Demo* const demo);

	const bool onEvent(Demo* const demo, const irr::SEvent& event);

private:

	irr::scene::IMeshSceneNode* sphere;

	irr::scene::ILightSceneNode* light;

	irr::gui::IGUIButton* startButton;

	irr::gui::IGUIButton* menuButton;

	irr::gui::IGUIButton* quitButton;
};

#endif /* MENUSTATE_H_ */
