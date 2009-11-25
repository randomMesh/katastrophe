// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef INTROSTATE_H_
#define INTROSTATE_H_

#include "../core/statemanager/State.h"

namespace irr
{
	class SEvent;
	namespace scene
	{
		class ISceneNode;
		class IBillboardTextSceneNode;
		class ISceneNodeAnimator;
	}

	namespace gui
	{
		class IGUIStaticText;
	}
}

class Demo;

class IntroState : public State<Demo, irr::SEvent>
{

public:

	IntroState();

	~IntroState();

	void onEnter(Demo* const game);

	void onLeave(Demo* const game);

	const bool onEvent(Demo* const game, const irr::SEvent& event);

	void onUpdate(Demo* const game);

private:

	irr::scene::ISceneNode* skybox;

	irr::scene::IBillboardTextSceneNode* introText;

	irr::scene::IBillboardTextSceneNode* copyRightText;

	irr::scene::ISceneNodeAnimator* anim1;

	irr::gui::IGUIStaticText* infoText;

	float pauseTimer;

	bool wait;
};

#endif /*INTROSTATE_H_*/
