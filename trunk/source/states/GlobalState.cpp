// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "GlobalState.h"

#include <IrrlichtDevice.h>
#include <ISceneManager.h>
#include <IGUIEnvironment.h>

#include "../core/Demo.h"
#include "../core/Configuration.h"

GlobalState::GlobalState() :
	lastFPS(0)
{

}

void GlobalState::onEnter(Demo* const demo)
{

}

void GlobalState::onLeave(Demo* const demo)
{
}

void GlobalState::onUpdate(Demo* const demo)
{
	irr::IrrlichtDevice* const device = demo->getDevice();
	irr::video::IVideoDriver* const driver = demo->getVideoDriver();

	if (device->isWindowActive())
	{
		// render the scene
		driver->beginScene(true, true, irr::video::SColor(255, 0, 0, 0));
		demo->rttCallBack();
		demo->getSceneManager()->drawAll();
		demo->drawCallBack();
		demo->getGuiEnvironment()->drawAll();
		driver->endScene();

		if (!device->isFullscreen())
		{
			const irr::s32 fps = driver->getFPS();
			if (this->lastFPS != fps)
			{
				this->lastFPS = fps;

				wchar_t fpsString[32];
				swprintf(fpsString, 32, L" [FPS = %u]", fps);

				irr::core::stringw fullTitle(demo->getConfiguration()->getProjectName());
				fullTitle.append(fpsString);

				device->setWindowCaption(fullTitle.c_str());
			}
		}
	}
}

const bool GlobalState::onEvent(Demo* const demo, const irr::SEvent& event)
{
	return false;
}
