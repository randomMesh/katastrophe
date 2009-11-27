// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "IntroState.h"
#include <IrrlichtDevice.h>
#include <ISceneManager.h>
#include <IMeshCache.h>
#include <ICameraSceneNode.h>
#include <IBillboardTextSceneNode.h>
#include <IParticleSystemSceneNode.h>
#include <IGUIEnvironment.h>
#include <IGUIStaticText.h>
#include "../core/Demo.h"
#include "../core/Configuration.h"

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

IntroState::IntroState() :
	introText(0), copyRightText(0), anim1(0),
	infoText(0), pauseTimer(5.0f), wait(false)
{

}

IntroState::~IntroState()
{
	if (this->introText)
	{
		this->introText->remove();
		this->introText->drop();
	}

	if (this->copyRightText)
	{
		this->copyRightText->remove();
		this->copyRightText->drop();
	}

	if (this->infoText)
	{
		this->infoText->remove();
		this->infoText->drop();
	}
}

void IntroState::onEnter(Demo* const demo)
{
	irr::video::IVideoDriver* const driver = demo->getVideoDriver();
	irr::scene::ISceneManager* const smgr = demo->getSceneManager();
	irr::gui::IGUIEnvironment* const guienv = demo->getGuiEnvironment();

	this->pauseTimer = 5.0f;
	this->wait = false;


	//hide mouse cursor
	demo->getDevice()->getCursorControl()->setVisible(false);

	//add a camera
	irr::scene::ICameraSceneNode* const camera = smgr->addCameraSceneNode();
	camera->setPosition(irr::core::vector3df(0.0f, 0.0f, 0.0f));
	camera->setTarget(irr::core::vector3df(0.0f, 0.0f, 1.0f));

	//create an animated skybox
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	irr::scene::ISceneNode* const skybox = smgr->addSkyBoxSceneNode(
		driver->getTexture("media/images/skybox/starfield/starfield_up.png"),
		driver->getTexture("media/images/skybox/starfield/starfield_dn.png"),
		driver->getTexture("media/images/skybox/starfield/starfield_lf.png"),
		driver->getTexture("media/images/skybox/starfield/starfield_rt.png"),
		driver->getTexture("media/images/skybox/starfield/starfield_ft.png"),
		driver->getTexture("media/images/skybox/starfield/starfield_bk.png"));
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);

	irr::scene::ISceneNodeAnimator* const skyAnim = smgr->createRotationAnimator(irr::core::vector3df(0.0f, 0.01f, 0.0f));
	skybox->addAnimator(skyAnim);
	skyAnim->drop();


	//add animated introText
	this->introText = smgr->addBillboardTextSceneNode(demo->getFont(), demo->getConfiguration()->getProjectName().c_str(), 0,
			irr::core::dimension2d<irr::f32>(40.0f, 4.0f), irr::core::vector3df(0.0f, 0.0f, 150.0f),
			-1,	irr::video::SColor(255, 255, 0, 0),	irr::video::SColor(255, 255, 233, 0));
	this->introText->grab();

	this->anim1 = smgr->createFlyStraightAnimator(
			irr::core::vector3df(0.0f, 0.0f, 150.0f), //start
			irr::core::vector3df(0.0f, 0.0f, 25.0f), //end
			4000, //time for way
			false); // loop mode

	this->introText->addAnimator(anim1);
	this->anim1->drop();


	//add copyright text
	this->copyRightText = smgr->addBillboardTextSceneNode(demo->getFont(), L"by randomMesh", 0,
			irr::core::dimension2d<irr::f32>(4.0f, 2.0f), irr::core::vector3df(0.0f, -20.0f, 15.0f),
			-1,	irr::video::SColor(255, 0, 0, 255), irr::video::SColor(255, 255, 255, 255));
	this->copyRightText->grab();


	irr::scene::IParticleSystemSceneNode* const ps = smgr->addParticleSystemSceneNode(false, this->introText);
	ps->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	ps->setMaterialTexture(0, driver->getTexture("media/images/particle.bmp"));
	ps->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);

	irr::scene::IParticleEmitter* const boxem = ps->createBoxEmitter(
		irr::core::aabbox3df(-18.0f, 0.0f,-3.0f, 20.0f, 3.0f, 1.0f),
		irr::core::vector3df(0.0f,0.005f,0.0f),
		150, 500,
		irr::video::SColor(255,0,0,0), irr::video::SColor(255,255,255,255),
		500, 1000,0,
		irr::core::dimension2df(0.5f, 0.5f), irr::core::dimension2df(1.0f, 1.0f));
	ps->setEmitter(boxem);
	boxem->drop();

	irr::scene::IParticleAffector* pa = 0;
	pa = ps->createFadeOutParticleAffector(irr::video::SColor(0, 0, 0, 0), 500);
	ps->addAffector(pa);
	pa->drop();

	pa = ps->createGravityAffector(irr::core::vector3df(0.0f, 0.006f, 0.0f), 2000);
	ps->addAffector(pa);
	pa->drop();





	irr::scene::IParticleEmitter* const em = ps->createPointEmitter(
		irr::core::vector3df(0.0f, 0.001f, 0.0f), 500, 600,
		irr::video::SColor(255, 0, 0, 128), irr::video::SColor(255, 0, 0, 0), 800, 1500, 60);
	em->setMinStartSize(irr::core::dimension2d<irr::f32>(0.5f, 0.5f));
	em->setMaxStartSize(irr::core::dimension2d<irr::f32>(1.0f, 1.0f));

	irr::scene::IParticleSystemSceneNode* const ps2 = smgr->addParticleSystemSceneNode(false, this->copyRightText);
	ps2->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	ps2->setPosition(irr::core::vector3df(-3.0f, 0.0f, 1.0f));
	ps2->setMaterialTexture(0, driver->getTexture("media/images/particle.bmp"));
	ps2->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
	ps2->setEmitter(em);

	irr::scene::IParticleSystemSceneNode* const ps3 = smgr->addParticleSystemSceneNode(false, this->copyRightText);
	ps3->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	ps3->setPosition(irr::core::vector3df(3.0f, 0.0f, 1.0f));
	ps3->setMaterialTexture(0, driver->getTexture("media/images/particle.bmp"));
	ps3->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
	ps3->setEmitter(em);

	em->drop();

	pa = ps2->createFadeOutParticleAffector(irr::video::SColor(0, 0, 0, 0), 500);
	ps2->addAffector(pa);
	pa->drop();

	pa = ps2->createGravityAffector(irr::core::vector3df(0.0f, -0.006f, 0.0f), 2000);
	ps2->addAffector(pa);
	pa->drop();

	pa = ps3->createFadeOutParticleAffector(irr::video::SColor(0, 0, 0, 0), 500);
	ps3->addAffector(pa);
	pa->drop();

	pa = ps3->createGravityAffector(irr::core::vector3df(0.0f, -0.006f, 0.0f), 2000);
	ps3->addAffector(pa);
	pa->drop();


	//add the info text
	this->infoText = guienv->addStaticText(L"Press [LMB] or [ESC] to skip intro",
		irr::core::rect<irr::s32>(SX(30), SY(20), SX(250), SY(40)), false, true, 0, -1, false);
	this->infoText->grab();
	this->infoText->setOverrideColor(irr::video::SColor(255, 0, 255, 255));


#ifdef _SOUND
	if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
		demo->getSoundEngine()->play2D("media/sounds/runwithme.ogg", true);
#endif
}

void IntroState::onLeave(Demo* const demo)
{
	irr::scene::ICameraSceneNode* const camera = demo->getSceneManager()->getActiveCamera();
	if (camera)
	{
		demo->getSceneManager()->setActiveCamera(0);
		camera->remove();
	}

	demo->getSceneManager()->getMeshCache()->clear();
	demo->getSceneManager()->clear();

	this->introText->removeAnimators();
	this->introText->remove();
	this->introText->drop();
	this->introText = 0;

	this->copyRightText->removeAnimators();
	this->copyRightText->remove();
	this->copyRightText->drop();
	this->copyRightText = 0;

	this->infoText->remove();
	this->infoText->drop();
	this->infoText = 0;

	irr::gui::ICursorControl* const cc = demo->getDevice()->getCursorControl();
	cc->setPosition(0.5f, 0.5f);	//center mouse for menu state
	cc->setVisible(true);			//set mouse cursor visible again
}

const bool IntroState::onEvent(Demo* const game, const irr::SEvent& event)
{
	switch(event.EventType)
	{
	case irr::EET_KEY_INPUT_EVENT:
	{
		if (!event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{
			case irr::KEY_ESCAPE: game->setState(game->findGameState("menu")); return true;
			default: return false;
			}
		}
	}
	break;

	case irr::EET_MOUSE_INPUT_EVENT:
	{
		if (irr::EMIE_LMOUSE_LEFT_UP == event.MouseInput.Event)
		{
			game->setState(game->findGameState("menu"));
			return true;;
		}
	}
	break;

	default: return false;
	}

	return false;
}

void IntroState::onUpdate(Demo* const demo)
{
	if (this->wait)
	{
		this->pauseTimer -= demo->getElapsed();
		if (this->pauseTimer <= 0)
		{
			//change gamestate to menu
			demo->setState(demo->findGameState("menu"));
		}
	}
	else
	{
		if (this->anim1->hasFinished())
		{
			this->wait = true;

			irr::scene::ISceneNodeAnimator* const anim2 = demo->getSceneManager()->createFlyStraightAnimator(
					this->copyRightText->getAbsolutePosition(), //start
					irr::core::vector3df(0.0f, -3.0f, 15.0f), //end
					2000, //time for way
					false); // loop mode

			this->copyRightText->addAnimator(anim2);
			anim2->drop();
		}
	}
}
