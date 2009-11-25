// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef APPLICATION_H_
#define APPLICATION_H_

namespace irr
{
	class IrrlichtDevice;
}

#ifdef _SOUND
namespace irrklang
{
	class ISoundEngine;
}
#endif

class Configuration;

class Application
{

public:

	Application();

	~Application();

	bool init(int argc, char* argv[]);

	///The main loop.
	void run() const;

private:

	///A pointer to the configuration object.
	Configuration* const configuration;

	///A pointer to the Irrlicht device.
	irr::IrrlichtDevice* device;

#ifdef _SOUND
	///A pointer to the irrklang engine.
	irrklang::ISoundEngine* soundEngine;
#endif
};

#endif /* APPLICATION_H_ */
