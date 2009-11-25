// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <irrString.h>
#include <SIrrCreationParameters.h>

/**
 * An object of this class holds all the variables the user may edit plus
 * some basic stuff like the name and version of the game.
 */
class Configuration
{

public:

	///The constructor sets up sane default values.
	Configuration();

	///Reads the configuration from an xml file.
	void readFromFile(const irr::c8* const fileName);

	///Writes the configuration to an xml file.
	void writeToFile(const irr::c8* const fileName) const;

	//\return Returns the project name.
	inline const irr::core::stringw& getProjectName() const
	{ return this->projectName; }

	//\return Returns the Irrlicht device parameters.
	inline const irr::SIrrlichtCreationParameters& getIrrlichtParams() const
	{ return this->params; }

	//\return Returns the gui transparence of the running state.
	inline const irr::u32 getGuiTransparency() const
	{ return this->guiTransparency; }

#ifdef _SOUND
	//\return True if user wants to hear music.
	inline const bool isSoundEnabled() const
	{ return this->soundEnabled; }

	//\return Returns the master volume. Range is from 0.0f to 1.0f
	inline irr::f32 getSoundVolume() const
	{ return this->volume; }
#endif

	inline const irr::f32 getCameraRotateSpeed() const
	{ return this->cameraRotateSpeed; }

	inline const irr::f32 getCameraMoveSpeed() const
	{ return this->cameraMoveSpeed; }

	inline const irr::f32 getCameraJumpSpeed() const
	{ return this->cameraJumpSpeed; }

	inline const bool isInvertMouse() const
	{ return this->invertMouse; }

	inline const irr::f32 getScatterFlockModifier() const
	{ return this->scatterFlockModifier; }

	inline const irr::f32 getSeekCenterOfMass() const
	{ return this->seekCenterOfMass; }

	inline const irr::f32 getDistanceToOtherBoids() const
	{ return this->distanceToOtherBoids; }

	inline const irr::f32 getMatchVelocity() const
	{ return this->matchVelocity; }

	inline const irr::f32 getTendencyTowardsPlace() const
	{ return this->tendencyTowardsPlace; }

	inline const irr::f32 getTendencyAvoidPlace() const
	{ return this->tendencyAvoidPlace; }

	inline const irr::f32 getMimimumAboveGround() const
	{ return this->minimumAboveGround; }

	inline const irr::f32 getSpeedLimit() const
	{ return this->speedLimit; }


	//setter

	inline void setIrrlichtParams(const irr::SIrrlichtCreationParameters& irrLichtParams)
	{ this->params = irrLichtParams; }

#ifdef _SOUND
	inline void setSoundEnabled(const bool enable)
	{ this->soundEnabled = enable; }

	inline void setSoundVolume(const irr::f32 volume)
	{ this->volume = volume; /*caution: we don't set the volume of the irrklang engine here*/ }
#endif

	inline void setScatterFlockModifier(const irr::f32 scatterFlockModifier)
	{ this->scatterFlockModifier = scatterFlockModifier; }

	inline void setSeekCenterOfMass(const irr::f32 seekCenterOfMass)
	{ this->seekCenterOfMass = seekCenterOfMass; }

	inline void setDistanceToOtherBoids(const irr::f32 distanceToOtherBoids)
	{ this->distanceToOtherBoids = distanceToOtherBoids; }

	inline void setMatchVelocity(const irr::f32 matchVelocity)
	{ this->matchVelocity = matchVelocity; }

	inline void setTendencyTowardsPlace(const irr::f32 tendencyTowardsPlace)
	{ this->tendencyTowardsPlace = tendencyTowardsPlace; }

	inline void setTendencyAvoidPlace(const irr::f32 tendencyAvoidPlace)
	{ this->tendencyAvoidPlace = tendencyAvoidPlace; }

	inline void setMinimumAboveGround(const irr::f32 minimumAboveGround)
	{ this->minimumAboveGround = minimumAboveGround; }

	inline void setSpeedLimit(const irr::f32 speedLimit)
	{
		this->speedLimit = speedLimit;
		if (this->speedLimit < 0.0f)
			this->speedLimit = 0.01f;
	}

	inline void setCameraRotateSpeed(const irr::f32 cameraRotateSpeed)
	{ this->cameraRotateSpeed = cameraRotateSpeed; }

	inline void setCameraMoveSpeed(const irr::f32 cameraMoveSpeed)
	{ this->cameraMoveSpeed = cameraMoveSpeed; }

	inline void setCameraJumpSpeed(const irr::f32 cameraJumpSpeed)
	{ this->cameraJumpSpeed = cameraJumpSpeed; }

	inline void setInvertMouse(const bool invert)
	{ this->invertMouse = invert; }

private:

	///Holds the name of the game.
	irr::core::stringw projectName;

	//stuff user can change

	///For Irrlicht device creation.
	irr::SIrrlichtCreationParameters params;

#ifdef _SOUND
	///true if player wants to hear sounds
	bool soundEnabled;

	///Volume 0 (silent) to 1.0f (full volume).
	irr::f32 volume;
#endif

	///Transparency of gui.
	irr::u32 guiTransparency;

	///Speed in degress with which the camera is rotated. This can be done only with the mouse.
	irr::f32 cameraRotateSpeed;

	///Speed in units per millisecond with which the camera is moved.
	irr::f32 cameraMoveSpeed;

	///Speed with which the camera is moved when jumping.
	irr::f32 cameraJumpSpeed;

	///Setting this to true makes the camera look up when the mouse is moved down and down when the mouse is moved up, the default is 'false' which means it will follow the movement of the mouse cursor.
	bool invertMouse;


	//flock

	irr::f32 scatterFlockModifier;

	irr::f32 seekCenterOfMass;

	irr::f32 distanceToOtherBoids;

	irr::f32 matchVelocity;

	irr::f32 tendencyTowardsPlace;

	irr::f32 tendencyAvoidPlace;

	irr::f32 minimumAboveGround;

	irr::f32 speedLimit;
};

#endif /*CONFIGURATION_H_*/
