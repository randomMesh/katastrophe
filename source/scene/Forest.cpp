// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "Forest.h"

#include <IrrlichtDevice.h>
#include <ISceneManager.h>
#include <ITerrainSceneNode.h>
#include <IBillboardSceneNode.h>

Forest::Forest(irr::IrrlichtDevice* const device, const irr::scene::ITerrainSceneNode* const terrain, const irr::core::array<irr::core::aabbox3df>& exceptions) :
	device(device), terrain(terrain), exceptions(exceptions)
{
	//store terrain size
	const irr::core::aabbox3df& tb = terrain->getBoundingBox();
	this->terrainSize.X = tb.getExtent().X;
	this->terrainSize.Y = tb.getExtent().Z;

	//add some trees to the pool

	Tree bleech;
	bleech.type = ETT_BLEECH;
	bleech.fileName = "media/images/trees/Bleech.png";
	bleech.dimension.Width = 768;
	bleech.dimension.Height = 1024;
	bleech.yOffset = 384.0f;
	treePool.push_back(bleech);

	Tree jubaea;
	jubaea.type = ETT_JUBAEA;
	jubaea.fileName = "media/images/trees/Greenhouse-Palm-jubaea01.png";
	jubaea.dimension.Width = 512;
	jubaea.dimension.Height = 512;
	jubaea.yOffset = 250.0f;
	this->treePool.push_back(jubaea);

	Tree bugainvillier;
	bugainvillier.type = ETT_BUGAINVILLIER;
	bugainvillier.fileName = "media/images/trees/Bougainvillier.png";
	bugainvillier.dimension.Width = 512;
	bugainvillier.dimension.Height = 512;
	bugainvillier.yOffset = 250.0f;
	this->treePool.push_back(bugainvillier);

	Tree flamboyant;
	flamboyant.type = ETT_FLAMBOYANT;
	flamboyant.fileName = "media/images/trees/Flamboyant.png";
	flamboyant.dimension.Width = 1024;
	flamboyant.dimension.Height = 768;
	flamboyant.yOffset = 300.0f;
	this->treePool.push_back(flamboyant);

	Tree taxus;
	taxus.type = ETT_TAXUS;
	taxus.fileName = "media/images/trees/taxus.png";
	taxus.dimension.Width = 256;
	taxus.dimension.Height = 512;
	taxus.yOffset = 0.0f;
	this->treePool.push_back(taxus);

	Tree cycas;
	cycas.type = ETT_CYCAS;
	cycas.fileName = "media/images/trees/cycas.png";
	cycas.dimension.Width = 1024;
	cycas.dimension.Height = 768;
	cycas.yOffset = 350.0f;
	this->treePool.push_back(cycas);
}

Forest::~Forest()
{
	for (irr::u32 i = 0; i< this->trees.size(); ++i)
	{
		this->trees[i]->remove();
	}
}

void Forest::addTree(const irr::core::vector3df& position, const E_TREE_TYPE& type)
{
	irr::scene::IBillboardSceneNode* const tree = this->device->getSceneManager()->addBillboardSceneNode(
		0, this->treePool[type].dimension, position);
	tree->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
	tree->setMaterialTexture(0, this->device->getVideoDriver()->getTexture(this->treePool[type].fileName));

	this->trees.push_back(tree);
}

void Forest::randomlyPlaceVegetation(const irr::u32 numTrees)
{
	//initialize random seed
	srand(this->device->getTimer()->getTime());

	irr::u32 i;
	for (i = 0; i < numTrees; ++i)
	{
		const irr::u32 type = rand()%6;

		irr::core::vector3df pos(makeRandomPos((E_TREE_TYPE)type));

		if (this->exceptions.size() > 0)
		{
			if (!isLegalPlace(pos))
			{
				//make new pos
				while (!isLegalPlace(pos))
				{
					pos = makeRandomPos((E_TREE_TYPE)type);
				}
			}
		}

		//NOTICE:	until other trees are pot, we only want ETT_BUGAINVILLIER, ETT_JUBAEA and ETT_TAXUS
		//			then just remove this notice
		switch(type)
		{
			case 0:	addTree(pos, ETT_BUGAINVILLIER); break;
			case 1:	addTree(pos, ETT_JUBAEA); break;
			case 2:	addTree(pos, ETT_TAXUS); break;
			case 3:	addTree(pos, ETT_BUGAINVILLIER); break;
			case 4:	addTree(pos, ETT_JUBAEA); break;
			case 5:	addTree(pos, ETT_TAXUS); break;
		}
		continue;
		//NOTICE END

		switch(type)
		{
			case 0:	addTree(pos, ETT_BLEECH); break;
			case 1:	addTree(pos, ETT_JUBAEA); break;
			case 2:	addTree(pos, ETT_BUGAINVILLIER); break;
			case 3:	addTree(pos, ETT_FLAMBOYANT); break;
			case 4:	addTree(pos, ETT_TAXUS); break;
			case 5:	addTree(pos, ETT_CYCAS); break;
		}
	}
}

void Forest::removeTree()
{
	const irr::u32 numTrees = this->trees.size();
	if (numTrees >= 1)
	{
		this->trees[numTrees-1]->remove();
		this->trees[numTrees-1]->drop();
		this->trees.erase(numTrees-1);
	}
}

bool Forest::isLegalPlace(const irr::core::vector3df& pos) const
{
	const irr::u32 numExceptions = exceptions.size();

	for (irr::u32 i = 0; i < numExceptions; ++i)
	{
		if (exceptions[i].isPointInside(pos))
		{
			return false;
		}
	}

	return true;
}

const irr::core::vector3df Forest::makeRandomPos(const E_TREE_TYPE& type) const
{
	const irr::f32 xPos = (rand()%(irr::s32)this->terrainSize.X);
	const irr::f32 zPos = (rand()%(irr::s32)this->terrainSize.Y);
	const irr::f32 yPos = (this->terrain->getHeight(xPos, zPos) + this->treePool[type].yOffset);

	return irr::core::vector3df(xPos, yPos, zPos);
}
