// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "CGrassGeneratorNode.h"
#include <IVideoDriver.h>
#include <ISceneManager.h>
#include <ITerrainSceneNode.h>
#include <ICameraSceneNode.h>

namespace irr
{
namespace scene
{

CGrassGeneratorNode::CGrassGeneratorNode(ISceneManager* const smgr, const bool autoSpawn) :
	irr::scene::ISceneNode(smgr->getRootSceneNode(), smgr, 0),
	currentMode(0),	//current material mode
	tex1(0),		//first texture
	tex2(0),		//second texture
	tex(false),		//texture swapping ^^
	Box(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
	frameCount(0),
	autoSpawn(autoSpawn)
{
#ifdef _DEBUG
	setDebugName("GrassGenerator");
#endif

	modes.push_back(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	modes.push_back(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
	modes.push_back(irr::video::EMT_TRANSPARENT_ADD_COLOR);
	modes.push_back(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);

	irr::video::IVideoDriver* driver = SceneManager->getVideoDriver();

	this->tex1 = driver->getTexture("media/images/grass/grass.png");
	this->tex2 = driver->getTexture("media/images/grass/grass2.bmp");
	driver->makeColorKeyTexture(this->tex2, irr::core::position2d<irr::s32>(0, 0));

	AutomaticCullingState = EAC_OFF;
//	DebugDataVisible = EDS_BBOX;
}

const bool CGrassGeneratorNode::addGrassToTerrain(
	irr::scene::IWindGenerator* const wind, irr::scene::ITerrainSceneNode* const terrain,
	const irr::video::IImage* const heightMap, const irr::video::IImage* const textureMap, const irr::video::IImage* const grassMap)
{
	//check for valid map files first
	if (!terrain || !heightMap || !textureMap || !grassMap)
	{
		//notify user
		return false;
	}

	this->removeAllGrass();

	//compute width and height
	const irr::u32 width = ((irr::u32)(heightMap->getDimension().Width*terrain->getScale().X))/GRASS_PATCH_SIZE;
	const irr::u32 height = ((irr::u32)(heightMap->getDimension().Height*terrain->getScale().Z))/GRASS_PATCH_SIZE;

//	printf("Width: %u\n", width);
//	printf("Height: %u\n", height);

	video::SMaterial material;
	material.TextureLayer[0].Texture = this->tex1;
	material.TextureLayer[0].TextureWrapU = video::ETC_CLAMP;
	material.TextureLayer[0].TextureWrapV = video::ETC_CLAMP;
	material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	material.MaterialTypeParam = 0.5f;
	material.Lighting = false;

	irr::scene::CGrassPatchSceneNode* grassNode = 0;
	// if we are not using auto spawn, we have to add grass nodes to cover the entire terrain.
	// if not, grass patch nodes are created and destroyed dynamically. NOT YET IMPLEMENTED
	// see CGrassGeneratorNode::OnRegisterSceneNode().
	if (!this->autoSpawn)
	{
		for (u32 x = 0; x < width; ++x)
		{
			for (u32 z = 0; z < height; ++z)
			{
				grassNode = new irr::scene::CGrassPatchSceneNode(terrain, SceneManager, 0,
					irr::core::vector3d<u32>(x, 0, z), "grass", heightMap, textureMap, grassMap, wind);
				grassNode->getMaterial(0) = material;

				this->grassNodes.push_back(grassNode);
				this->addChild(grassNode);
				grassNode->updateAbsolutePosition();
				grassNode->drop();
			}
		}
	}

	if (this->grassNodes.size())
	{
		this->Box = this->grassNodes[0]->getTransformedBoundingBox();

		for (u32 i = 1; i < this->grassNodes.size(); ++i)
		{
			core::aabbox3df box = grassNode->getTransformedBoundingBox();
			this->Box.addInternalBox(box);
		}
	}

	//grass looks better with, but could break rendering of other nodes. be sure you use the proper ZWriteEnable flag
	this->SceneManager->getParameters()->setAttribute(irr::scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);

	return true;
}

const void CGrassGeneratorNode::removeAllGrass()
{
	const irr::u32 size = this->grassNodes.size();

	irr::u32 i;
	for (i = 0; i < size; ++i)
	{
		this->grassNodes[i]->remove();
		this->grassNodes[i] = 0;
	}

	this->grassNodes.clear();
}

const void CGrassGeneratorNode::swapTextures()
{
	if (this->grassNodes.size() == 0)
		return;

	this->tex = !this->tex;

	for (irr::u32 n = 0; n < this->grassNodes.size(); ++n)
		this->grassNodes[n]->setMaterialTexture(0, this->tex ? this->tex2 : this->tex1);
}

const void CGrassGeneratorNode::cycleModes(const bool forward)
{
	const irr::u32 numGrass = this->grassNodes.size();

	if (numGrass == 0)
		return;

	if (forward)
	{
		if (this->currentMode < this->modes.size() - 1)
			++this->currentMode;
		else this->currentMode = 0;
	}
	else
	{
		if (this->currentMode > 0)
			--this->currentMode;
		else this->currentMode = this->modes.size() - 1;
	}

	for (irr::u32 n = 0; n < numGrass; ++n)
		this->grassNodes[n]->setMaterialType(this->modes[this->currentMode]);
}

const void CGrassGeneratorNode::decreaseDrawDistance(const irr::f32 dist) const
{
	for (irr::u32 n = 0; n < this->grassNodes.size(); ++n)
	{
		if (this->grassNodes[n]->getDrawDistance() > GRASS_PATCH_SIZE + dist)
			this->grassNodes[n]->setDrawDistance(this->grassNodes[n]->getDrawDistance() - dist);
	}
}

const void CGrassGeneratorNode::increaseDrawDistance(const irr::f32 dist) const
{
	for (irr::u32 n = 0; n < this->grassNodes.size(); ++n)
	{
		grassNodes[n]->setDrawDistance(grassNodes[n]->getDrawDistance() + dist);
	}
}

const void CGrassGeneratorNode::decreaseWindResolution(const irr::u32 delta) const
{
	if (this->grassNodes.size() == 0)
		return;

	if (this->grassNodes[0]->getWindRes() > delta)
	{
		for (irr::u32 n = 0; n < this->grassNodes.size(); ++n)
		{
			this->grassNodes[n]->setWindRes(this->grassNodes[n]->getWindRes() - delta);
		}
	}
}

const void CGrassGeneratorNode::increaseWindResolution(const irr::u32 delta) const
{
	if (this->grassNodes.size() == 0) return;

	if (grassNodes[0]->getWindRes() < 20)
	{
		irr::u32 n;
		for (n = 0; n < this->grassNodes.size(); ++n)
		{
			grassNodes[n]->setWindRes(grassNodes[n]->getWindRes() + delta);
		}
	}
}

const void CGrassGeneratorNode::decreaseRefreshDelay() const
{
	if (this->grassNodes.size() == 0) return;

	if (this->grassNodes[0]->getRefreshDelay() > 4)
	{
		irr::u32 n;
		for (n = 0; n < this->grassNodes.size(); ++n)
		{
			this->grassNodes[n]->setRefreshDelay(this->grassNodes[n]->getRefreshDelay() - 10);
		}
	}
}

const void CGrassGeneratorNode::increaseRefreshDelay() const
{
	if (this->grassNodes.size() == 0) return;

	if (this->grassNodes[0]->getRefreshDelay() < 500)
	{
		irr::u32 n;
		for (n = 0; n < this->grassNodes.size(); ++n)
		{
			this->grassNodes[n]->setRefreshDelay(this->grassNodes[n]->getRefreshDelay() + 10);
		}
	}
}

const void CGrassGeneratorNode::decreaseMaxDensity(const irr::u32 delta) const
{
	if (this->grassNodes.size() == 0) return;

	irr::u32 n;
	for (n = 0; n < this->grassNodes.size(); ++n)
	{
		if ((this->grassNodes[n]->getMaxDensity() - delta) > 5)
			this->grassNodes[n]->setMaxDensity(this->grassNodes[n]->getMaxDensity() - delta);
	}
}

const void CGrassGeneratorNode::increaseMaxDensity(const irr::u32 delta) const
{
	if (this->grassNodes.size() == 0) return;

	irr::u32 n;
	for (n = 0; n < this->grassNodes.size(); ++n)
	{
		this->grassNodes[n]->setMaxDensity(this->grassNodes[n]->getMaxDensity() + delta);
	}
}

void CGrassGeneratorNode::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		if (this->autoSpawn && ++this->frameCount == 100) //every 100th frame
		{
			//culling of grass patches
			irr::scene::ICameraSceneNode* const camera = SceneManager->getActiveCamera();
			irr::core::vector3df cameraPos = camera->getAbsolutePosition();
			irr::core::vector3df cameraTarget = camera->getTarget();

			//spawning and destroying grass nodes

			this->frameCount = 0;
		}

		SceneManager->registerNodeForRendering(this);
	}

	irr::scene::ISceneNode::OnRegisterSceneNode();
}

void CGrassGeneratorNode::render()
{
	if (DebugDataVisible & irr::scene::EDS_BBOX)
	{
		video::IVideoDriver* const driver = SceneManager->getVideoDriver();

		video::SMaterial mat;
		mat.Lighting = false;
		driver->setMaterial(mat);
		driver->setTransform(video::ETS_WORLD, core::matrix4());
		driver->draw3DBox(Box, video::SColor(0, 126, 255, 50));
	}
}

const irr::core::aabbox3d<irr::f32>& CGrassGeneratorNode::getBoundingBox() const
{
	return Box;
}

void CGrassGeneratorNode::addChild(irr::scene::ISceneNode* child)
{
	//if (child->getType() == irr::scene::ESNT_GRASSPATCH)
	irr::scene::ISceneNode::addChild(child);
}

void CGrassGeneratorNode::setScale(const core::vector3df& scale)
{
	//not allowed
}

void CGrassGeneratorNode::setRotation(const core::vector3df& rotation)
{
	//not allowed
}

}
}
