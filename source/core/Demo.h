// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef DEMO_H_
#define DEMO_H_

#include <ITimer.h>
#include <irrArray.h>
#include <IEventReceiver.h>
#include "statemanager/StateManager.h"
#include <SMaterial.h>

namespace irr
{
	class IrrlichtDevice;
	class SEvent;

	namespace scene
	{
		class ISceneManager;
		class IMesh;
	}

	namespace gui
	{
		class IGUIEnvironment;
		class IGUIFont;
	}

	namespace video
	{
		class IVideoDriver;
	}
}

#ifdef _SOUND
namespace irrklang
{
	class ISoundEngine;
}
#endif

class Configuration;
class MTRand;

// these macros will return the scaled value based on current screen size
// (i assume 1024x768 is the normal res) to provide a resolution independent gui.
#define SX(val) ( (irr::u32)(driver->getScreenSize().Width*(irr::f32)(val)/1024.0f) )
#define SY(val) ( (irr::u32)(driver->getScreenSize().Height*(irr::f32)(val)/768.0f) )

class Demo : public irr::IEventReceiver, public StateManager<Demo, irr::SEvent>
{

public:

	Demo(Configuration* const configuration, irr::IrrlichtDevice* const device
#ifdef _SOUND
		, irrklang::ISoundEngine* const soundEngine
#endif
	);

	~Demo();

	///Adapt irrlicht event handling to ours.
	bool OnEvent(const irr::SEvent& event)
	{
		return onEvent(event);
	}

	void addGameState(const char* const name, State<Demo, irr::SEvent>* state);

	/**
	 * Finds a game state by its name.
	 * \return The game state. 0 if no such game state exits.
	 */
	State<Demo, irr::SEvent>* const findGameState(const char* const name);

	inline void tick()
	{
		//compute elapsed time since last frame
		this->now = this->timer->getTime();
		this->elapsed = ((irr::f32)(this->now - this->then))*0.001f;
		this->then = this->now;
	}

	///Takes a screenshot from the last rendered frame.
	void takeScreenshot() const;


	///Called from Irrlichts render loop in order to let the game render to texture.
	void rttCallBack();

	///Called from Irrlichts render loop in order to let the game draw.
	void drawCallBack();

	///Enable texture filtering. //TODO: doesn't apply for newly created nodes. should use global configuration
	void setTextureFiltering(
		const bool bilinear = true,
		const bool trilinear = false,
		const irr::u16 anisotropic = 0) const;


	irr::scene::IMesh* const createBoidMesh(
		const irr::f32 size = 1.0f,
		const irr::video::SColor& headColor = irr::video::SColor(255, 255, 0, 0),
		const irr::video::SColor& bodyColor = irr::video::SColor(255, 255, 255, 0)) const;

	//getter

	inline Configuration* const getConfiguration() const { return this->configuration; }

	///\return The Irrlicht device.
	inline irr::IrrlichtDevice* getDevice() const { return this->device; }

#ifdef _SOUND
	inline irrklang::ISoundEngine* const getSoundEngine() const { return this->soundEngine; }
#endif

	///\return The Irrlicht scene manager.
	inline irr::scene::ISceneManager* const getSceneManager() const { return this->sceneManager; }

	///\return The Irrlicht gui environment.
	inline irr::gui::IGUIEnvironment* const getGuiEnvironment() const { return this->guiEnvironment; }

	///\return The Irrlicht video driver.
	inline irr::video::IVideoDriver* const getVideoDriver() const { return this->videoDriver; }

	///\return The default font of the demo.
	inline irr::gui::IGUIFont* const getFont() const { return this->font; }

	///\return The elapsed time.
	inline const irr::f32 getElapsed() const { return this->elapsed; }

	inline MTRand* const getRandomNumberGenerator() const { return this->rng; }

private:

	///Set the default font of the demo.
	void setFont(const irr::c8* const filename);

	///Defines a state for the state machine.
	struct SState
	{
		SState(const irr::c8* name = 0, State<Demo, irr::SEvent>* state = 0) :
			name(name), state(state)
		{

		}

		bool operator<(const SState& other) const
		{
			return strcmp(name, other.name) < 0;
		}

		const irr::c8* name;

		State<Demo, irr::SEvent>* state;
	};

	///Holds all game states.
	irr::core::array<SState> gameStates;

	///A reference to the configuation object
	Configuration* const configuration;

	///A pointer to the Irrlicht device.
	irr::IrrlichtDevice* const device;
#ifdef _SOUND
	irrklang::ISoundEngine* const soundEngine;
#endif
	///A pointer to the Irrlicht scene manager.
	irr::scene::ISceneManager* const sceneManager;

	///A pointer to the Irrlicht video driver.
	irr::video::IVideoDriver* const videoDriver;

	///A pointer to the Irrlicht gui environment.
	irr::gui::IGUIEnvironment* const guiEnvironment;

	///Default font.
	irr::gui::IGUIFont* font;

	///A pointer to the Irrlicht timer.
	irr::ITimer* const timer;
	irr::u32 then;
	irr::u32 now;
	irr::f32 elapsed;

	//Material for debug drawing.
	irr::video::SMaterial debugMat;

	///Mersenne twister.
	MTRand* rng;
};

#endif /* DEMO_H_ */
