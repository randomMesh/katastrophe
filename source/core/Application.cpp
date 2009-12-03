// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "Application.h"
#include <irrlicht.h>
#ifdef _SOUND
#include <irrKlang.h>
#endif
#include "Configuration.h"
#include "Demo.h"
#include "../states/GlobalState.h"
#include "../states/IntroState.h"
#include "../states/MenuState.h"
#include "../states/OptionsState.h"
#include "../states/RunningState.h"

Application::Application() :
	configuration(new Configuration),
	device(0)
#ifdef _SOUND
	,soundEngine(0)
#endif
{

}

Application::~Application()
{
	delete this->configuration;
}

bool Application::parseCommandLine(int argc, char* argv[])
{
	//for now just use it for writing the boid mesh to file
	if (argc > 1)
	{
		if (strcmp(argv[1],"-v") == 0 || strcmp(argv[1],"--version") == 0)
		{
			printf("Katastrophe V. 0.02\n");
		}
		else if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0)
		{
			printf("Sorry, no help yet\n");
		}
		else if(strcmp(argv[1],"-w") == 0)
		{
			irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_NULL);
			Demo demo(0, device, 0);

			irr::scene::IMesh* boidMesh = demo.createBoidMesh(1.0f);

			irr::scene::IMeshWriter* mw = device->getSceneManager()->createMeshWriter(irr::scene::EMWT_OBJ);
			irr::io::IWriteFile* file = device->getFileSystem()->createAndWriteFile("mesh.obj");
			mw->writeMesh(file, boidMesh);

			file->drop();
			mw->drop();
			boidMesh->drop();
			device->drop();
		}

		return false;
	}

	return true;
}

bool Application::init(int argc, char* argv[])
{
	if (!parseCommandLine(argc, argv))
		return false;

	//read the configuration file
	this->configuration->readFromFile("config.xml");

	//init Irrlicht
	this->device = irr::createDeviceEx(this->configuration->getIrrlichtParams());
	if (this->device == 0)
		return false;

	//make irrlicht blabber less
	this->device->getLogger()->setLogLevel(irr::ELL_WARNING);

#ifdef _SOUND
	//init irrklang
	this->soundEngine = irrklang::createIrrKlangDevice();
	if (!this->soundEngine)
	{
		// error starting up the engine
		this->device->getLogger()->log(L"Disabling sound.", irr::ELL_WARNING);
		this->configuration->setSoundEnabled(false);
	}
	else
		this->soundEngine->setSoundVolume(this->configuration->getSoundVolume());
#endif

	return true;
}

void Application::run() const
{
	//create global game state
	GlobalState global;

	// create all other game states the demo can be in
	IntroState intro;
	MenuState menu;
	OptionsState options;
	RunningState running;

	//create the state machine
	Demo demo(this->configuration, this->device
#ifdef _SOUND
		, this->soundEngine
#endif
	);

	this->device->drop(); //demo grabs the engine pointers, so drop them
#ifdef _SOUND
	this->soundEngine->drop();
#endif

	//register game states
	demo.addGameState("intro", &intro);
	demo.addGameState("menu", &menu);
	demo.addGameState("options", &options);
	demo.addGameState("running", &running);

	// setup the state machine
	demo.setGlobalState(&global);

	// start the game with an intro
	demo.setState(&intro);

	//main loop
	irr::IrrlichtDevice* const device = demo.getDevice();
	while (device->run())
	{
		//update demo timer
		demo.tick();

		//update the state machine (aka rendering and event receiving)
		demo.onUpdate();
	}

	//clean up current state
	demo.getCurrentState()->onLeave(&demo);
}
