// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "TeleporterSceneNode.h"

#include <ISceneManager.h>
#include <IVideoDriver.h>
#include <ISceneNodeAnimatorCollisionResponse.h>
#include <ICameraSceneNode.h>

#include "../core/Configuration.h"
#include "../core/Demo.h"
#include "WhirlSceneNode.h"

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

namespace irr
{
namespace scene
{

TeleporterSceneNode::TeleporterSceneNode(
	const Demo* const demo,
	const core::aabbox3d<f32>& placeA, const core::aabbox3d<f32>& placeB,
	const bool viceversa,
	const bool useLookDirA, const bool useLookDirB,
	const core::vector3df& placeALookDir, const core::vector3df& placeBLookDir,
	const s32 id) :
	ISceneNode(demo->getSceneManager()->getRootSceneNode(), demo->getSceneManager(), id),
	demo(demo),
	placeA(placeA), placeB(placeB), viceversa(viceversa),
	useLookDirA(useLookDirA), useLookDirB(useLookDirB),
	placeALookDir(placeALookDir), placeBLookDir(placeBLookDir),
	dontTeleport(false), dontTeleportTimer(0.0f)
{
#ifdef _DEBUG
	setDebugName("TeleporterSceneNode");
#endif

	irr::scene::ISceneManager* const smgr = demo->getSceneManager();

	this->whirl[0] = new WhirlSceneNode(100, smgr->getRootSceneNode(), smgr, id);
	this->whirl[0]->setPosition(placeA.getCenter());
	this->whirl[0]->setScale(irr::core::vector3df(30.0f, 30.0f, 30.0f));
	//this->whirl[0]->setDebugDataVisible(irr::scene::EDS_BBOX);

	this->whirl[1] = new WhirlSceneNode(100, smgr->getRootSceneNode(), smgr, id);
	this->whirl[1]->setPosition(placeB.getCenter());
	this->whirl[1]->setScale(irr::core::vector3df(30.0f, 30.0f, 30.0f));
	//this->whirl[1]->setDebugDataVisible(irr::scene::EDS_BBOX);


	this->Material.Lighting = false;


	smgr->addBillboardTextSceneNode(demo->getFont(), L"Teleporter", this->whirl[0],
		core::dimension2df(120.0f, 100.0f), core::vector3df(0.0f, 5.0f, 0.0f), 0);

	smgr->addBillboardTextSceneNode(demo->getFont(), L"Teleporter", this->whirl[1],
		core::dimension2df(120.0f, 100.0f), core::vector3df(0.0f, 5.0f, 0.0f), 0);


	this->AutomaticCullingState = EAC_OFF;
}

TeleporterSceneNode::~TeleporterSceneNode()
{
	this->whirl[0]->remove();
	this->whirl[0]->drop();

	this->whirl[1]->remove();
	this->whirl[1]->drop();
}

void TeleporterSceneNode::OnRegisterSceneNode()
{
	if (this->IsVisible)
		this->SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

const core::aabbox3d<f32>& TeleporterSceneNode::getBoundingBox() const
{
	return this->Box;
}

irr::u32 TeleporterSceneNode::getMaterialCount() const
{
	return 1;
}

irr::video::SMaterial& TeleporterSceneNode::getMaterial(irr::u32 i)
{
	return this->Material;
}

void TeleporterSceneNode::OnAnimate(irr::u32 timeMs)
{

	if (!this->dontTeleport)
	{
		irr::core::list<irr::scene::ISceneNode* const>::Iterator it = this->nodesToWatch.begin();
		const irr::core::list<irr::scene::ISceneNode* const>::Iterator& end = this->nodesToWatch.end();
		for(; it != end; ++it)
		{
			irr::scene::ISceneNode* const node = (*it);

			if (this->placeA.isPointInside(node->getAbsolutePosition()))
			{
				//teleport from A to B
				teleport(node, true);
			}

			else if (this->viceversa && placeB.isPointInside(node->getAbsolutePosition()))
			{
				//teleport from B to A
				teleport(node, false);
			}
		}
	}
	else
	{
		this->dontTeleportTimer -= this->demo->getElapsed();

		if (this->dontTeleportTimer <= 0.0f)
		{
			this->dontTeleport = false;

			this->whirl[0]->setVisible(true);
			this->whirl[1]->setVisible(true);
		}
	}

	ISceneNode::OnAnimate(timeMs);
}

void TeleporterSceneNode::render()
{
	if (DebugDataVisible & EDS_BBOX)
	{
		video::SColor color(255, 255, 255, 0);
		if (this->dontTeleport)
			color.setGreen(0);

		video::IVideoDriver* const driver = this->SceneManager->getVideoDriver();
		driver->setMaterial(this->Material);
		driver->setTransform(irr::video::ETS_WORLD, core::matrix4());

		//draw placeA
		driver->draw3DBox(this->placeA, color);

		//draw placeB
		driver->draw3DBox(this->placeB, color);
	}
}

void TeleporterSceneNode::addNodeToWatchList(ISceneNode* const node)
{
	this->nodesToWatch.push_back(node);
}

void TeleporterSceneNode::removeNodeFromWatchList(const ISceneNode* const node)
{
	core::list<ISceneNode* const>::Iterator it = this->nodesToWatch.begin();
	const core::list<ISceneNode* const>::Iterator& end = this->nodesToWatch.end();
	for(; it != end; ++it)
	{
		if (node == (*it))
		{
			this->nodesToWatch.erase(it);
			break;
		}
	}
}

void TeleporterSceneNode::teleport(ISceneNode* const node, const bool fromAToB)
{
	//'teleport' the node
	node->setPosition(fromAToB ? this->placeB.getCenter() : this->placeA.getCenter());

	//if it's a camera, set animator target
	if (node == SceneManager->getActiveCamera())
	{
		if (this->useLookDirB && fromAToB)
		{
			node->updateAbsolutePosition();
			((scene::ICameraSceneNode*)node)->setTarget(this->placeBLookDir);
		}

		else if (this->useLookDirA && !fromAToB)
		{
			node->updateAbsolutePosition();
			((scene::ICameraSceneNode*)node)->setTarget(this->placeALookDir);
		}
	}

	//now the interesting stuff: update animators if any
	const core::list<ISceneNodeAnimator*> anims = node->getAnimators();

	core::list<ISceneNodeAnimator*>::ConstIterator it = anims.begin();
	const core::list<ISceneNodeAnimator*>::ConstIterator& end = anims.end();
	for(; it != end; ++it)
	{
		ISceneNodeAnimator* const anim = (*it);
		const irr::scene::ESCENE_NODE_ANIMATOR_TYPE& type = anim->getType();

		switch (type)
		{

		case ESNAT_COLLISION_RESPONSE:
		{
			ISceneNodeAnimatorCollisionResponse* const col = (ISceneNodeAnimatorCollisionResponse* const)anim;
			col->setTargetNode(node);
		}
		break;

		default:
			break;
		}

	}


	this->dontTeleport = true;
	this->dontTeleportTimer = 3.0f;

	this->whirl[0]->setVisible(false);
	this->whirl[1]->setVisible(false);

#ifdef _SOUND
	irrklang::ISoundEngine* const soundEngine = this->demo->getSoundEngine();

	//play teleport sound
	if (soundEngine && this->demo->getConfiguration()->isSoundEnabled())
	{
		irrklang::ISound* snd = 0;
		snd = soundEngine->play3D("media/sounds/teleport.wav", this->placeA.getCenter(), false, true);
		if (snd)
		{
			snd->setMinDistance(500.0f);
			snd->setIsPaused(false);
			snd->drop();
			snd = 0;
		}

		snd = soundEngine->play3D("media/sounds/teleport.wav", this->placeB.getCenter(), false, true);
		if (snd)
		{
			snd->setMinDistance(500.0f);
			snd->setIsPaused(false);
			snd->drop();
			snd = 0;
		}
	}
#endif
}

} // end namespace scene
} // end namespace irr
