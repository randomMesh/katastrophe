// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

// Original code and artwork by Paulo Oliveira (ProSoft, http://br.geocities.com/paulo_cmv/)
// The code is Public Domain.
// With updates from gammaray and torleif

#ifndef _CLENSFLARESCENENODE_H_INCLUDED_
#define _CLENSFLARESCENENODE_H_INCLUDED_

#include "ILensFlareSceneNode.h"
#include "S3DVertex.h"

namespace irr
{
namespace scene
{

class ISceneCollisionManager;

/** CLensFlareSceneNode

Extends functionality to ILensFlareSceneNode

 */

class CLensFlareSceneNode : public ILensFlareSceneNode
{

public:

	// constructor
	CLensFlareSceneNode(
		ISceneNode* parent, ISceneManager* mgr, s32 id=-1,
		const core::vector3df& position = core::vector3df());

	u32 getMaterialCount() const { return 1; }

	// Returns SMaterial list
	video::SMaterial& getMaterial(u32 i) { return Material; }

	// Scene creation function
	void OnRegisterSceneNode();

	// Upon draw
	void render();

	// returns a bounding box where it is
	const core::aabbox3d<f32>& getBoundingBox() const;

	// Resets screen size
	void setScreenSize(const core::dimension2d<s32> &);

private:

	// Size of screen in pixels
	core::dimension2d<s32> Screensize;

	// starting pos of camera
	core::vector3df StartPosition;

	// Bounding box
	core::aabbox3d<f32> BBox;

	// Internal 'list' of textures used
	video::SMaterial Material;

	// Matrix of each textures position
	video::S3DVertex vertices[4];

	// Where to draw each node
	u16 indices[6];

	ISceneCollisionManager* collmgr;
};

} // end namespace scene
} // end namespace irr

#endif
