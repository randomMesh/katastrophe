// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "RunningState.h"

#include <IrrlichtDevice.h>
#include <ISceneManager.h>
#include <ICameraSceneNode.h>
#include <ITerrainSceneNode.h>
#include <ISceneNodeAnimatorCameraFPS.h>
#include <IGUIEnvironment.h>
#include <IGUIStaticText.h>
#include <IGUISkin.h>

#include "../core/Demo.h"
#include "../core/Configuration.h"
#include "../core/Map.h"
#include "../core/Flock.h"
#include "../scene/grass/CGrassGeneratorNode.h"
#include "../scene/Forest.h"
#include "../gui/Compass.h"

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

RunningState::RunningState() :
	map(0),
	pgCompass(0), infoText(0),
	mousePos(0, 0), rightMouseButton(false),
	freezeTarget(false)
{

}

RunningState::~RunningState()
{
	if (this->pgCompass)
	{
		this->pgCompass->remove();
		this->pgCompass->drop();
	}

	if (this->infoText)
	{
		this->infoText->remove();
		this->infoText->drop();
	}

	delete this->map;
}

void RunningState::onEnter(Demo* const demo)
{
	const Configuration* const config = demo->getConfiguration();
	irr::video::IVideoDriver* const driver = demo->getVideoDriver();
	irr::gui::IGUIEnvironment* const guienv = demo->getGuiEnvironment();

	this->freezeTarget = false;

	//clear key array and mouse bool
	memset(this->keys, 0, sizeof(bool)*irr::KEY_KEY_CODES_COUNT);
	this->rightMouseButton = false;

	//hide mouse cursor
	demo->getDevice()->getCursorControl()->setVisible(false);


	// change gui transparency
	irr::gui::IGUISkin* const currentSkin = guienv->getSkin();
	irr::u32 c;
	for (c = 0; c < irr::gui::EGDC_COUNT; ++c)
	{
		irr::video::SColor col = currentSkin->getColor(irr::gui::EGUI_DEFAULT_COLOR(c));
		col.setAlpha(config->getGuiTransparency());
		currentSkin->setColor(irr::gui::EGUI_DEFAULT_COLOR(c), col);
	}


	//add info text
	this->infoText = guienv->addStaticText(L"", irr::core::rect<irr::s32>(SX(20), SY(20), SX(450), SY(550)), false, true, 0, -1, true);
	this->infoText->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	this->infoText->setOverrideColor(irr::video::SColor(255, 0, 255, 255));
	this->infoText->grab();


	// add a compass
	const irr::core::dimension2di compassSize(SX(128), SX(128));
	const irr::core::rect<irr::s32> compassRect(SX(880), SY(10), SX(880) + compassSize.Width, SY(10) + compassSize.Height);

	this->pgCompass = new irr::gui::Compass(compassRect, guienv, guienv->getRootGUIElement());
	this->pgCompass->SetCompassBodyTexture(driver->getTexture("media/images/compass/compass_body.png"));
	this->pgCompass->SetCompassNeedleTexture(driver->getTexture("media/images/compass/compass_needle.png"));


	//load a map
	this->map = new Map(demo);
	this->map->load("");	//load default map


#ifdef _SOUND
	irrklang::ISoundEngine* const soundEngine = demo->getSoundEngine();

	if (soundEngine && config->isSoundEnabled())
	{
		if (!soundEngine->isCurrentlyPlaying("media/sounds/runwithme.ogg"))
			soundEngine->play2D("media/sounds/runwithme.ogg", true);

		this->map->startSounds();
	}
#endif
}

void RunningState::onLeave(Demo* const demo)
{
	this->pgCompass->remove();
	this->pgCompass->drop();
	this->pgCompass = 0;

	this->infoText->remove();
	this->infoText->drop();
	this->infoText = 0;

	delete this->map;
	this->map = 0;

	//clear gui environment
	demo->getGuiEnvironment()->clear();

	//remove textures
//	demo->getVideoDriver()->removeAllTextures();

	//unhide mouse cursor
	demo->getDevice()->getCursorControl()->setVisible(true);
}

const bool RunningState::onEvent(Demo* const demo, const irr::SEvent& event)
{
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
		if (!event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{
			case irr::KEY_ESCAPE: demo->setState(demo->findGameState("menu")); return true;

			case irr::KEY_F1:
			{
				this->map->getTerrain()->setMaterialFlag(irr::video::EMF_WIREFRAME, !this->map->getTerrain()->getMaterial(0).Wireframe);
			}
			return true;

			case irr::KEY_F2:
			{
				this->map->getGrass()->setVisible(!this->map->getGrass()->isVisible());
			}
			return true;



			case irr::KEY_F3:
			{
				this->map->getForest()->removeTree();
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
			}
			return true;


			case irr::KEY_F4:
			{
				this->map->getForest()->randomlyPlaceVegetation(1);
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
			}
			return true;




#ifdef _SOUND
			case irr::KEY_F5:
			{
				//toggle sounds
				const bool sound = demo->getConfiguration()->isSoundEnabled();
				demo->getConfiguration()->setSoundEnabled(!sound);

				if (sound)
					demo->getSoundEngine()->stopAllSounds();
				else
				{
					//start music again
					if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					{
						demo->getSoundEngine()->play2D("media/sounds/runwithme.ogg", true);
						map->startSounds();
					}
				}
			}
			return true;
#endif

			case irr::KEY_F6: demo->takeScreenshot(); return true;


			case irr::KEY_PAUSE: //fall through
			case irr::KEY_KEY_P: this->map->getFlock()->setPaused(!this->map->getFlock()->isPaused()); return true;

			case irr::KEY_KEY_I:
			{
				const bool invert = demo->getConfiguration()->isInvertMouse();
				demo->getConfiguration()->setInvertMouse(!invert);

				//update camera animator
				irr::scene::ISceneNodeAnimatorCameraFPS* const animFPS = (irr::scene::ISceneNodeAnimatorCameraFPS* const)(*demo->getSceneManager()->getActiveCamera()->getAnimators().begin());
				animFPS->setInvertMouse(!invert);
			}
			return true;

			case irr::KEY_PLUS: this->map->addBoid(); return true;
			case irr::KEY_MINUS: this->map->removeBoid(this->map->getFlock()->getBoids().getLast()); return true;

			case irr::KEY_KEY_H: this->infoText->setVisible(!this->infoText->isVisible()); return true;

			default: break;
			}
		}

		keys[event.KeyInput.Key] = event.KeyInput.PressedDown;
	}




	else if(irr::EET_MOUSE_INPUT_EVENT == event.EventType)
	{

		if (irr::EMIE_MOUSE_MOVED == event.MouseInput.Event)
		{
			this->mousePos.X = event.MouseInput.X;
			this->mousePos.Y = event.MouseInput.Y;

			return false; //if we return true here, our fps camera doesn't work anymore
		}

		//check right mouse button
		if (irr::EMIE_RMOUSE_PRESSED_DOWN == event.MouseInput.Event)
		{
			this->rightMouseButton = true;

			return true;
		}
		else if (irr::EMIE_RMOUSE_LEFT_UP == event.MouseInput.Event)
		{
			this->rightMouseButton = false;

			return true;
		}


		//check middle mouse button
		else if (irr::EMIE_MMOUSE_LEFT_UP == event.MouseInput.Event)
		{
			this->freezeTarget = !this->freezeTarget;

			return true;
		}

		//check mouse wheel
		else if (irr::EMIE_MOUSE_WHEEL == event.MouseInput.Event)
		{
			irr::scene::ICameraSceneNode* const camera = demo->getSceneManager()->getActiveCamera();

			//zoom camera
			irr::f32 newFOV = camera->getFOV();

			if (event.MouseInput.Wheel < 0)
				newFOV = irr::core::min_(newFOV + irr::core::DEGTORAD, irr::core::PI*0.5f);
			else
				newFOV = irr::core::max_(newFOV - irr::core::DEGTORAD, irr::core::PI*0.0125f);

			camera->setFOV(newFOV);

			return true;
		}
	}


	return false;
}

void RunningState::onUpdate(Demo* const demo)
{
	const Configuration* const config = demo->getConfiguration();
	const irr::scene::ICameraSceneNode* const camera = demo->getSceneManager()->getActiveCamera();
	const irr::core::vector3df& camPos = camera->getPosition();
	const irr::core::vector3df& camTarget = camera->getTarget();

#ifdef _SOUND
	//update sound listener
	if (demo->getSoundEngine() && config->isSoundEnabled())
		demo->getSoundEngine()->setListenerPosition(camPos, (camTarget - camPos));//.normalize());
#endif

	//update compass
	irr::core::vector3df vec(0.0f, 0.0f, 1.0f);
	camera->getAbsoluteTransformation().rotateVect(vec);
	this->pgCompass->SetCompassHeading(vec.getHorizontalAngle().Y);

	//update map
	this->map->update(this->freezeTarget, camPos, camTarget, this->rightMouseButton);



	//update info text
	if (this->infoText->isVisible())
	{
		const Flock* const flock = this->map->getFlock();

		irr::core::stringw t(
			L"                                                  Help\n\n"
			" Player:\n"
			"   WASD / Arrows - Move\n"
			"   SPACE - Jump\n"
			"   Wheel - Zoom\n"
			"           I - Invert: ");
		t.append(config->isInvertMouse() ? L"(On)" : L"(Off)\n");


		t.append(L"   Position: X:");
		t.append(irr::core::stringw(camPos.X));
		t.append(L" | Y:");
		t.append(irr::core::stringw(camPos.Y));
		t.append(L" | Z:");
		t.append(irr::core::stringw(camPos.Z));
		t.append(L"\n   Target: X:");
		t.append(irr::core::stringw(camTarget.X));
		t.append(L" | Y:");
		t.append(irr::core::stringw(camTarget.Y));
		t.append(L" | Z:");
		t.append(irr::core::stringw(camTarget.Z));

		t.append(L"\n\n Flock:\n"
			"   Flock rules - See options\n"
			"   Point mouse - Set flock target\n"
			"   Middle mouse button - Freeze flock target\n"
			"   Right mouse button - Scatter flock\n"
			"   - / + - Number of boids: ");
		t.append(irr::core::stringw(flock->getBoids().size()));
		t.append(L"\n   P - ");	t.append(flock->isPaused() ? L"Unpause" : L"Pause"); t.append(L" flock\n");

		const irr::core::vector3df& flockTarget = flock->getTarget();
		t.append(L"   Target: X:");
		t.append(irr::core::stringw(flockTarget.X));
		t.append(L" | Y:");
		t.append(irr::core::stringw(flockTarget.Y));
		t.append(L" | Z:");
		t.append(irr::core::stringw(flockTarget.Z));

		t.append(L"\n\n Demo:\n"
			"   F1 - Terrain wireframe : ");
		t.append(this->map->getTerrain()->getMaterial(0).Wireframe ? L"(On)" : L"(Off)");
		t.append(L"\n   F2 - Toggle grass: ");
		t.append(irr::core::stringw(this->map->getGrass()->isVisible()? L"(On)" : L"(Off)"));
		t.append(L"\n   F3 / F4 - Number of trees: ");
		t.append(irr::core::stringw(this->map->getForest()->getNumberOfTrees()));
		t.append(L"\n   F5 - Toggle sounds: ");
#ifdef _SOUND
		t.append(config->isSoundEnabled() ? L"(On)" : L"(Off)");
#else
		t.append(L"(Sounds disabled)");
#endif
		t.append(L"\n   F6 - Take screenshot\n");
		t.append(L"\n   H - Hide help\n"
			"   ESC - Back to menu");

		this->infoText->setText(t.c_str());
	}
}

void RunningState::drawCallback(Demo* const demo) const
{
	this->map->drawDebug();
}
