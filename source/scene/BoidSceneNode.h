// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef BOIDSCENENODE_H_
#define BOIDSCENENODE_H_

#include <ISceneNode.h>
#include <S3DVertex.h>

namespace irrklang
{
	class ISoundEngine;
}

namespace irr
{

namespace scene
{

class IMeshBuffer;

/**
 * This class implemnts a single boid in a flock
 *
 * See http://www.vergenet.net/~conrad/boids/pseudocode.html
 *
 * Rules of a boid in a flock:
 *  Separation: steer to avoid crowding local flockmates
 *  Alignment: steer towards the average heading of local flockmates
 *  Cohesion: steer to move toward the average position of local flockmates
 */
class BoidSceneNode : public ISceneNode
{

public:

	BoidSceneNode(
		IMeshBuffer* const shape,
		const core::vector3df& position,
		const irr::f32 borders[4],
		const f32 mimimumAboveGround,
		ISceneManager* const mgr);

	~BoidSceneNode();

	//To compare boids. ID is set to ++boidID, do not change!
	bool operator==(const BoidSceneNode& other) const { return (this->ID == other.ID); }
	bool operator!=(const BoidSceneNode& other) const { return !(*this == other); }

	void OnAnimate(u32 timeMs);

	void render();

	const core::aabbox3d<float>& getBoundingBox() const;

	void OnRegisterSceneNode();

	video::SMaterial& getMaterial(u32 i);

	u32 getMaterialCount() const;

	/// The ID is currently abused in order for operator== to work. No changes allowed.
	void setID(s32 id) { }

	void applyRules(
		irr::scene::ITriangleSelector* const selector,
		const irr::core::array<BoidSceneNode*>& boids,
		const irr::f32 distanceToOtherBoids,
		const irr::f32 seekCenterOfMass,
		const irr::f32 matchVelocity,
		const irr::core::vector3df& target,
		const irr::f32 tendencyTowardsPlace,
		const irr::f32 tendencyAvoidPlace,
		const bool scatterFlock,
		const irr::f32 scatterFlockModifier,
		const irr::f32 deltaTime,
		const irr::f32 speedLimit
#ifdef _SOUND
		, irrklang::ISoundEngine* const soundEngine = 0, const bool soundEnabled = false
#endif
	);

	inline const core::vector3df& getVelocity() const { return this->velocity; }

	inline const irr::core::line3d<f32>& getGroundRay() const { return this->groundRay; }

	///Does a simple bounding-sphere collision detection.
	//\param other The BoidSceneNode to test.
	//\param radius The radius of the sphere.
	//\return true if a collision occured.
	bool isInNeighborhood(const BoidSceneNode* const other, const f32 radius) const;

private:

	static u32 boidID;

	//! The three boids rules sufficiently demonstrate a complex emergent flocking behaviour.
	/// They are all that is required to simulate a distributed, leaderless flocking behaviour.
	const core::vector3df doReynolds(
		const core::array<BoidSceneNode*>& boids,
		const f32 distanceToOtherBoids,
		const f32 seekCenterOfMass,
		const f32 matchVelocity,
		const bool scatterFlock,
		const f32 scatterFlockModifier) const;

	///Makes the boid fly towards the target.
	//\return The offset vector to be added to the velocity.
	const core::vector3df seekTarget(const core::vector3df& target, const f32 tendencyTowardsPlace) const;

	const core::vector3df bindPosition(scene::ITriangleSelector* const selector, const f32 tendencyTowardsPlace, const f32 tendencyAvoidPlace);

	void limitSpeed(const f32 speedLimit);


	void startPerching(const core::vector3df& outCollisionPoint);

	void stopPerching();


	///The shape of the boid.
	IMeshBuffer* const shape;

	///Material to draw the meshbuffer.
	video::SMaterial material;


	///The normals of the mesh
	irr::video::S3DVertex* normalsVertices;
	u32 numVertices;
	irr::u16* indices;
	u32 numIndices;

	///Material to draw the normals.
	video::SMaterial normalsMaterial;


	///The current speed of a boid. Is set by applying the rules to the boid.
	core::vector3df velocity;



	///Boid is currently on ground if true
	bool perching;

	///Time to stay on ground in ms
	f32 perchTimer;

	///Time the boid is not allowed to perch again after perching.
	f32 dontPerchTimer;



	///Xmin, Xmax, Zmin, Zmax
	f32 borders[4];

	f32 mimimumAboveGround;


	///The ray to test for terrain collision.
	core::line3d<f32> groundRay;


	bool firstUpdate;
	u32 lastAnimationTime;
	bool forward;
};

}
}
#endif /* BOIDSCENENODE_H_ */
