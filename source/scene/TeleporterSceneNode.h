// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef TELEPORTERSCENENODE_H_
#define TELEPORTERSCENENODE_H_

#include <ISceneNode.h>

class Demo;

namespace irr
{
	namespace scene
	{
		class WhirlSceneNode;

class TeleporterSceneNode : public irr::scene::ISceneNode
{

public:

	TeleporterSceneNode(
		const Demo* const demo,
		const core::aabbox3d<f32>& placeA,
		const core::aabbox3d<f32>& placeB,
		const bool viceversa = true,
		const bool useLookDirA = false, //use current target
		const bool useLookDirB = false, //use current target
		const core::vector3df& placeALookDir = irr::core::vector3df(0.0f, 0.0f, 0.0f),
		const core::vector3df& placeBLookDir = irr::core::vector3df(0.0f, 0.0f, 0.0f),
		const s32 id = -1);

	~TeleporterSceneNode();

	u32 getMaterialCount() const;


	void OnRegisterSceneNode();

	void OnAnimate(u32 timeMs);

	void render();

	const core::aabbox3d<f32>& getBoundingBox() const;

	video::SMaterial& getMaterial(u32 i);


	void addNodeToWatchList(irr::scene::ISceneNode* const node);

	void removeNodeFromWatchList(const irr::scene::ISceneNode* const node);

private:

	void teleport(ISceneNode* const node, const bool fromAToB);

	const Demo* const demo;

	///A list of all scene nodes which are able to teleport with this teleporter.
	core::list<ISceneNode* const> nodesToWatch;


	///This box defines the area of place A.
	core::aabbox3df placeA;

	///This box defines the area of place B.
	core::aabbox3df placeB;


	bool viceversa;
	bool useLookDirA;
	bool useLookDirB;

	///The new target of the camera after teleportation to place A.
	core::vector3df placeALookDir;

	///The new target of the camera after teleportation to place B.
	core::vector3df placeBLookDir;



	bool dontTeleport;

	f32 dontTeleportTimer;

	WhirlSceneNode* whirl[2];

	video::SMaterial Material;

	core::aabbox3d<f32> Box;


	bool firstUpdate;
	u32 lastAnimationTime;
};

} // end namespace scene
} // end namespace irr

#endif /* TELEPORTERSCENENODE_H_ */
