// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "WhirlSceneNode.h"
#include <IVideoDriver.h>
#include <ISceneManager.h>
#include <ICameraSceneNode.h>

namespace irr
{
namespace scene
{

WhirlSceneNode::WhirlSceneNode(const u32 num, ISceneNode* const parent, ISceneManager* const mgr, s32 id, const f32 distance) :
	ISceneNode(parent, mgr, id),
	num(num),

	speed(200), diff(0), firstUpdate(true), lastScaleTime(0)
{

#ifdef _DEBUG
	setDebugName("WhirlSceneNode");
#endif

	this->stars.set_used(this->num);

	for(u32 i = 0; i < num; ++i)
	{
		this->stars[i].color = video::SColor(255, rand()%256, rand()%256, rand()%256);
		this->stars[i].angle = 0.0f;
		this->stars[i].dist = (f32(i)/num)*distance;
	}

	this->Material.Lighting = false;
	this->Material.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
	this->Material.TextureLayer[0].Texture = SceneManager->getVideoDriver()->getTexture("media/images/star.bmp");;
	this->Material.ZWriteEnable = false;

	this->vertices[0] = video::S3DVertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, video::SColor(), 0.0f, 0.0f); //top left
	this->vertices[1] = video::S3DVertex( 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, video::SColor(), 1.0f, 0.0f); //top right
	this->vertices[2] = video::S3DVertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, video::SColor(), 0.0f, 1.0f); //bottom left
	this->vertices[3] = video::S3DVertex( 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, video::SColor(), 1.0f, 1.0f); //bottom right

	this->indices[0] = 0;
	this->indices[1] = 1;
	this->indices[2] = 2;

	this->indices[3] = 2;
	this->indices[4] = 1;
	this->indices[5] = 3;


	this->Box.reset(this->vertices[0].Pos);
	for (u32  i = 1; i < 4; ++i)
		this->Box.addInternalPoint(this->vertices[i].Pos);
}

void WhirlSceneNode::OnRegisterSceneNode()
{
	if (this->IsVisible)
		this->SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

const core::aabbox3d<f32>& WhirlSceneNode::getBoundingBox() const
{
	return this->Box;
}

u32 WhirlSceneNode::getMaterialCount() const
{
	return 1;
}

video::SMaterial& WhirlSceneNode::getMaterial(irr::u32 i)
{
	return this->Material;
}

void WhirlSceneNode::OnAnimate(u32 timeMs)
{
	if (this->firstUpdate)
	{
		this->lastScaleTime = timeMs;
		this->firstUpdate = false;
	}

	this->diff = (timeMs - this->lastScaleTime)*0.001;
	this->lastScaleTime = timeMs;

	//always face camera
	this->RelativeRotation = core::vector3df(
		this->AbsoluteTransformation.getTranslation() - SceneManager->getActiveCamera()->getAbsolutePosition()).getHorizontalAngle();

	ISceneNode::OnAnimate(timeMs);
}


void WhirlSceneNode::render()
{
	video::IVideoDriver* const driver = this->SceneManager->getVideoDriver();
	driver->setMaterial(this->Material);

	core::matrix4 rot, pos, world;

	for(u32 i = 0; i < this->num; ++i)
	{
		this->vertices[0].Color = this->stars[i].color;
		this->vertices[1].Color = this->stars[i].color;
		this->vertices[2].Color = this->stars[i].color;
		this->vertices[3].Color = this->stars[i].color;

		this->stars[i].angle += ((f32)i)/(this->num*100)*speed*diff;

		pos.setTranslation(core::vector3df(this->stars[i].dist, 0.0f, 0.0f));
		rot.setRotationRadians(core::vector3df(0.0f, 0.0f, this->stars[i].angle));
		world = this->AbsoluteTransformation*rot*pos*rot;

		driver->setTransform(video::ETS_WORLD, world);
		driver->drawIndexedTriangleList(&this->vertices[0], 4, &this->indices[0], 2);

		if (DebugDataVisible & scene::EDS_BBOX)
		{
			video::SMaterial mat;
			mat.Lighting = false;
			driver->setMaterial(mat);
			driver->draw3DBox(this->Box, video::SColor(this->stars[i].color));

			driver->setMaterial(this->Material); //reset
		}
	}
}

} // end namespace scene
} // end namespace irr
