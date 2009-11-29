// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "Flock.h"
#include <ISceneManager.h>
#include "Configuration.h"
#include "Demo.h"
#include "../scene/BoidSceneNode.h"
#include "MersenneTwister.h"

Flock::Flock(const Demo* const demo, const irr::core::vector3df& target, const irr::f32 borders[4]) :
	demo(demo), paused(false), target(target),

	scatterFlockModifier(demo->getConfiguration()->getScatterFlockModifier()),
	seekCenterOfMass(demo->getConfiguration()->getSeekCenterOfMass()),
	distanceToOtherBoids(demo->getConfiguration()->getDistanceToOtherBoids()),
	matchVelocity(demo->getConfiguration()->getMatchVelocity()),
	tendencyTowardsPlace(demo->getConfiguration()->getTendencyTowardsPlace()),
	tendencyAvoidPlace(demo->getConfiguration()->getTendencyAvoidPlace()),
	speedLimit(demo->getConfiguration()->getSpeedLimit())
{
	memcpy(this->borders, borders, sizeof(irr::f32)*4);
}

Flock::~Flock()
{
	const irr::u32 numBoids = boids.size();

	for (irr::u32 i = 0; i < numBoids; ++i)
	{
		boids[i]->remove();
		boids[i]->drop();
	}
}

void Flock::update(irr::scene::ITriangleSelector* const selector, const bool scatterFlock) const
{
	if (this->paused)
		return;

	//update every boid
	const irr::u32 numBoids = this->boids.size();
	const irr::f32 deltaTime = this->demo->getElapsed();

#ifdef _SOUND
	irrklang::ISoundEngine* const soundEngine = this->demo->getSoundEngine();
	const bool soundEnabled = this->demo->getConfiguration()->isSoundEnabled();
#endif

	irr::u32 current;
	for (current = 0; current < numBoids; ++current)
	{
		//apply rules to boid
		this->boids[current]->applyRules(
			selector,
			this->boids,

			//rules in flock
			this->distanceToOtherBoids,
			this->seekCenterOfMass,
			this->matchVelocity,

			//seek and avoid targets
			this->target,
			this->tendencyTowardsPlace,
			this->tendencyAvoidPlace,

			//scattering the flock
			scatterFlock, this->scatterFlockModifier,

			//timing
			deltaTime, this->speedLimit
#ifdef _SOUND
			, soundEngine, soundEnabled
#endif
		);
	}
}

irr::scene::BoidSceneNode* const Flock::addBoid(irr::scene::IMesh* const boidMesh)
{
	MTRand& rng = demo->getRandomNumberGenerator();

	const irr::f32 posX = (float)rng.randInt(1500) + 1500;
	const irr::f32 posY = (float)rng.randInt(1500) + 1500;
	const irr::f32 posZ = (float)rng.randInt(1500) + 1500;
	const irr::f64 percent = rng.rand();
	const bool w = percent < 0.5 ? true : false;

	irr::scene::BoidSceneNode* const boid = new irr::scene::BoidSceneNode(boidMesh,
		target + irr::core::vector3df(w ? posX : -posX, posY, w ? -posZ : posZ), this->borders, this->demo->getConfiguration()->getMimimumAboveGround(),
		demo->getSceneManager());

	this->boids.push_back(boid);

	return boid;
}

bool Flock::removeBoid(irr::scene::BoidSceneNode* const boid)
{
	const irr::u32 numBoids = this->boids.size();

	if (numBoids > 2) // there should be at least 2 boids in the flock
	{
		for(irr::u32 i = 0; i < numBoids; ++i)
		{
			if (*this->boids[i] == *boid)
			{
				boid->remove();
				boid->drop(); //?
				this->boids.erase(i);

				return true;
			}
		}
	}

	return false;
}
