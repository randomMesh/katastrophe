// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef WHIRLSCENENODE_H_
#define WHIRLSCENENODE_H_

#include <ISceneNode.h>
#include <S3DVertex.h>

namespace irr
{
namespace scene
{

/**
 * Based off of the code found here: http://lesson.ifdef.jp/009.html
 */
class WhirlSceneNode : public irr::scene::ISceneNode
{

public:

	WhirlSceneNode(const u32 num, ISceneNode* const parent, ISceneManager* const mgr, s32 id, const f32 distance = 2.5f);

	void OnRegisterSceneNode();

	void OnAnimate(u32 timeMs);

	void render();

	const core::aabbox3d<f32>& getBoundingBox() const;

	u32 getMaterialCount() const;

	video::SMaterial& getMaterial(u32 i);

private:

	video::S3DVertex vertices[4];

	u16 indices[6];

	video::SMaterial Material;

	core::aabbox3d<f32> Box;

	u32 num;

	struct Star
	{
		video::SColor color;
		f32 dist;
		f32 angle;
	};

	core::array<Star> stars;
};

} // end namespace scene
} // end namespace irr

#endif /* WHIRLSCENENODE_H_ */
