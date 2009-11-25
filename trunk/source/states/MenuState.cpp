// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "MenuState.h"
#include <IrrlichtDevice.h>
#include <ISceneManager.h>
#include <IMeshCache.h>
#include <ILightSceneNode.h>
#include <IGUIEnvironment.h>
#include <IGUIButton.h>
#include <ICameraSceneNode.h>
#include <IMeshSceneNode.h>
#include "../core/Demo.h"
#include "../core/Configuration.h"

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

MenuState::MenuState() :
	sphere(0), light(0),
	startButton(0), menuButton(0), quitButton(0)
{

}

void MenuState::onEnter(Demo* const demo)
{
	irr::scene::ISceneManager* const smgr = demo->getSceneManager();
	irr::video::IVideoDriver* const driver = demo->getVideoDriver();
	irr::gui::IGUIEnvironment* const guienv = demo->getGuiEnvironment();


	//set gui to opaque
	irr::gui::IGUISkin* const currentSkin = guienv->getSkin();
	irr::u32 c;
	for (c = 0; c < irr::gui::EGDC_COUNT; ++c)
	{
		irr::video::SColor col = currentSkin->getColor(irr::gui::EGUI_DEFAULT_COLOR(c));
		col.setAlpha(255);
		currentSkin->setColor(irr::gui::EGUI_DEFAULT_COLOR(c), col);
	}

	//add a camera
	irr::scene::ICameraSceneNode* const camera = smgr->addCameraSceneNode();
	camera->setPosition(irr::core::vector3df(0.0f, 0.0f, -30.0f));


	//add a sphere
	this->sphere = smgr->addSphereSceneNode(10, 64);
	this->sphere->setMaterialTexture(0, driver->getTexture("media/images/particle.bmp"));
	this->sphere->setMaterialTexture(1, driver->getTexture("media/images/temporal-wake.jpg"));
	this->sphere->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	this->sphere->setMaterialType(irr::video::EMT_SOLID_2_LAYER);
	this->sphere->getMaterial(0).Shininess = 32.0f;
	this->sphere->getMaterial(0).getTextureMatrix(0).setScale(4.0f);
	this->sphere->getMaterial(0).getTextureMatrix(1).setScale(2.0f);

	irr::scene::ISceneNodeAnimator* anim = 0;
	anim = smgr->createRotationAnimator(irr::core::vector3df(0.5f, 0.5f, 0.5f));
	this->sphere->addAnimator(anim);
	anim->drop();

	anim = smgr->createFlyStraightAnimator(irr::core::vector3df(0.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f, 0.0f, 40.0f), 4000, true, true);
	this->sphere->addAnimator(anim);
	anim->drop();


	// add lights
	this->light = smgr->addLightSceneNode(0, irr::core::vector3df(50, 50, -50), irr::video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 60.0f);

	smgr->setAmbientLight(irr::video::SColorf(0.9f, 0.9f, 0.9f));


	irr::video::ITexture* const buttonTexture = driver->getTexture("media/images/button.png");
	if (buttonTexture)
	{
		const irr::core::dimension2du& screenSize = driver->getScreenSize();
		const irr::core::dimension2du& buttonSize = buttonTexture->getSize();
		const irr::s32 buttonX = (screenSize.Width - buttonSize.Width)/2;

		this->startButton = guienv->addButton(irr::core::rect<irr::s32>(buttonX, SY(230), buttonX + buttonSize.Width, SY(230) + buttonSize.Height), 0, -1, L"[S]tart");
		this->startButton->setImage(buttonTexture);
		this->startButton->setUseAlphaChannel(true);
		this->startButton->setDrawBorder(false);

		this->menuButton = guienv->addButton(irr::core::rect<irr::s32>(buttonX, SY(280), buttonX + buttonSize.Width, SY(280) + buttonSize.Height), 0, -1, L"[O]ptions");
		this->menuButton->setImage(buttonTexture);
		this->menuButton->setUseAlphaChannel(true);
		this->menuButton->setDrawBorder(false);

		this->quitButton = guienv->addButton(irr::core::rect<irr::s32>(buttonX, SY(330), buttonX + buttonSize.Width, SY(330) + buttonSize.Height), 0, -1, L"[Q]uit");
		this->quitButton->setImage(buttonTexture);
		this->quitButton->setUseAlphaChannel(true);
		this->quitButton->setDrawBorder(false);
	}
	else
	{
		demo->getDevice()->getLogger()->log("Error: Missing texture.", irr::ELL_ERROR);
	}
}

void MenuState::onLeave(Demo* const demo)
{
	//clear scenemanager
	demo->getSceneManager()->getMeshCache()->clear();
	demo->getSceneManager()->clear();
	this->sphere = 0;
	this->light = 0;

	//clear gui
	demo->getGuiEnvironment()->clear();
	this->startButton = 0;
	this->menuButton = 0;
	this->quitButton = 0;

	//remove textures
	demo->getVideoDriver()->removeAllTextures();
}

void MenuState::onUpdate(Demo* const demo)
{

}

const bool MenuState::onEvent(Demo* const demo, const irr::SEvent& event)
{
	switch(event.EventType)
	{

	case irr::EET_KEY_INPUT_EVENT:
	{
		if (!event.KeyInput.PressedDown)
		{
			if (event.KeyInput.Key == irr::KEY_KEY_S)
			{
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
				demo->setState(demo->findGameState("running"));
				return true;
			}

			else if (event.KeyInput.Key == irr::KEY_KEY_O)
			{
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
				demo->setState(demo->findGameState("options"));
				return true;
			}

			else if (event.KeyInput.Key == irr::KEY_KEY_Q)
			{
				demo->getDevice()->closeDevice();
				return true;
			}
		}
	}
	break;


	case irr::EET_GUI_EVENT:
	{
		if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == this->startButton)
			{
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
				demo->setState(demo->findGameState("running"));
				return true;
			}
			else if (event.GUIEvent.Caller == this->menuButton)
			{
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
				demo->setState(demo->findGameState("options"));
				return true;
			}
			else if (event.GUIEvent.Caller == this->quitButton)
			{
				demo->getDevice()->closeDevice();
				return true;
			}
		}


		else if (event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED)
		{
			const irr::gui::EGUI_ELEMENT_TYPE& type = event.GUIEvent.Caller->getType();

			if  (type == irr::gui::EGUIET_BUTTON)
			{
				((irr::gui::IGUIButton*)event.GUIEvent.Caller)->setPressed(true);
				return true;
			}
		}

		else if (event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT)
		{
			const irr::gui::EGUI_ELEMENT_TYPE& type = event.GUIEvent.Caller->getType();

			if  (type == irr::gui::EGUIET_BUTTON)
			{
				((irr::gui::IGUIButton*)event.GUIEvent.Caller)->setPressed(false);
				return true;
			}
		}

	}
	break;

	default:
		return false;
	}

	return false;
}
