// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "Demo.h"
#include <IrrlichtDevice.h>
#include <ISceneManager.h>
#include <IGUIEnvironment.h>

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

#include "MersenneTwister.h"
#include <ctime>

Demo::Demo(Configuration* const configuration, irr::IrrlichtDevice* const device
#ifdef _SOUND
		, irrklang::ISoundEngine* const soundEngine
#endif
) :
StateManager<Demo, irr::SEvent>(this),

configuration(configuration), device(device),
#ifdef _SOUND
soundEngine(soundEngine),
#endif
sceneManager(device->getSceneManager()),
videoDriver(device->getVideoDriver()),
guiEnvironment(device->getGUIEnvironment()),

font(0),

timer(device->getTimer()),
then(timer->getTime()),
now(0),
elapsed(0.0f)
{
	//grab pointers to engines
	device->grab();
#ifdef _SOUND
	soundEngine->grab();
#endif

	//tell irrlicht that we are the event receiver
	this->device->setEventReceiver(this);

	//setup default demo font
	this->setFont("media/font/fontlucida.png");

	//setup material for debugging
	this->debugMat.Lighting = false;

	this->rng = new MTRand(time(NULL));
}

Demo::~Demo()
{
	//release engine pointers
	this->device->drop();
#ifdef _SOUND
	this->soundEngine->drop();
#endif

	delete this->rng;
}

void Demo::addGameState(const char* const name, State<Demo, irr::SEvent>* const state)
{
	this->gameStates.push_back(SState(name, state));
}

State<Demo, irr::SEvent>* const Demo::findGameState(const char* const name)
{
	const irr::s32 f = this->gameStates.binary_search(SState(name));
	if (f != -1)
		return this->gameStates[f].state;

	return 0;
}

void Demo::setFont(const irr::c8* const filename)
{
	this->font = this->guiEnvironment->getFont(filename);
	if (this->font)
		this->guiEnvironment->getSkin()->setFont(this->font);
	else
	{
		this->device->getLogger()->log("Warning: Couldn't set font. Using default.", irr::ELL_WARNING);
		this->font = this->guiEnvironment->getBuiltInFont();
	}
}

void Demo::takeScreenshot() const
{
	//get image from the last rendered frame
	irr::video::IImage* const image = this->videoDriver->createScreenShot();

	if (image) //should always be true, but you never know. ;)
	{
		//construct a filename, consisting of local time and file extension
		irr::c8 filename[64];
		snprintf(filename, 64, "capture/screenshot_%u.png", this->timer->getRealTime());

		//write screenshot to file
		if (!this->videoDriver->writeImageToFile(image, filename))
			this->device->getLogger()->log(L"Failed to take screenshot. Maybe you need to create the capture folder.", irr::ELL_WARNING);

		//Don't forget to drop image since we don't need it anymore.
		image->drop();
	}
}

void Demo::rttCallBack()
{
	this->currentState->rttCallback(this);
}

void Demo::drawCallBack()
{
	this->videoDriver->setMaterial(this->debugMat);
	this->videoDriver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
	this->currentState->drawCallback(this);
}

void Demo::setTextureFiltering(const bool bilinear, const bool trilinear, const irr::u16 anisotropic) const
{
	// find all nodes
	irr::core::array<irr::scene::ISceneNode*> nodes;
	this->sceneManager->getSceneNodesFromType(irr::scene::ESNT_ANY, nodes);

	const irr::u32 numNodes = nodes.size();

	for (irr::u32 n = 0; n < numNodes; ++n)
	{
		irr::scene::ISceneNode* const node = nodes[n];

//		node->setAutomaticCulling(irr::scene::EAC_OFF);


//		const irr::scene::ESCENE_NODE_TYPE& type = node->getType();
//		const irr::s32 id = node->getID();
//		const irr::c8* name = node->getName();

		const irr::u32 numMaterial = node->getMaterialCount();
		for (irr::u32 m = 0; m < numMaterial; ++m)
		{
			irr::video::SMaterial& mat = node->getMaterial(m);

			for (irr::u32 t = 0; t < irr::video::MATERIAL_MAX_TEXTURES; ++t)
			{
				mat.TextureLayer[t].BilinearFilter = bilinear;
				mat.TextureLayer[t].TrilinearFilter = trilinear;
				mat.TextureLayer[t].AnisotropicFilter = anisotropic;
			}
		}

	}
}
