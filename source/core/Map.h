// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef MAP_H_
#define MAP_H_

#include <vector3d.h>
#include <irrArray.h>
#include <irrString.h>

namespace irr
{
	namespace scene
	{
		class ISceneCollisionManager;
		class IMetaTriangleSelector;
		class ISceneNodeAnimatorCollisionResponse;

		class ITerrainSceneNode;
		class CGrassGeneratorNode;
		class TeleporterSceneNode;
		class IParticleSystemSceneNode;
		class IBillboardSceneNode;
		class IVolumeLightSceneNode;

		class BoidSceneNode;
	}
}

class Demo;
class Flock;
class Forest;

#ifdef _SOUND
namespace irrklang
{
	class ISound;
}
#endif

class Map
{

public:

	Map(Demo* const demo);

	~Map();

	bool load(const irr::core::stringc& fileName);

	void update(
		const bool freezeTarget,
		const irr::core::vector3df& camPos,
		const irr::core::vector3df& camTarget,
		const bool rightMouseButton);

	///Adds a boid to the map.
	//\return A pointer to the new boid.
	irr::scene::BoidSceneNode* const addBoid();

	///Removes a boid from the map.
	//\param The boid to remove.
	bool removeBoid(irr::scene::BoidSceneNode* const boid);

#ifdef _SOUND
	void startSounds();

	void stopSounds();
#endif

	inline Flock* const getFlock() const { return this->flock; }

	inline irr::scene::ITerrainSceneNode* const getTerrain() const { return this->terrain; }

	inline irr::scene::CGrassGeneratorNode* const getGrass() const { return this->grassGeneratorNode; }

	inline Forest* const getForest() const { return this->forest; }

	void drawDebug() const;

private:

	void clear();

	void loadDefault();

	Demo* const demo;

	Flock* flock;



	///Initial player postion.
	irr::core::vector3df playerStartPosition;

	///Initial player target.
	irr::core::vector3df playerStartTarget;



	irr::scene::ISceneCollisionManager* collmgr;

	irr::scene::IMetaTriangleSelector* selector;

	irr::scene::ISceneNodeAnimatorCollisionResponse* anim;


	irr::scene::IParticleSystemSceneNode* ps;

	irr::scene::ITerrainSceneNode* terrain;

	Forest* forest;

	irr::core::array<irr::scene::TeleporterSceneNode*> teleporters;

	irr::scene::CGrassGeneratorNode* grassGeneratorNode;

	irr::scene::IVolumeLightSceneNode* cursor;

	irr::scene::IBillboardSceneNode* crosshair;


#ifdef _SOUND
	irrklang::ISound* wind;
#endif
};

#endif /* MAP_H_ */
