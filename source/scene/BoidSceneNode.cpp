// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "BoidSceneNode.h"
#include <ISceneManager.h>
#include <IVideoDriver.h>
#include <ISceneCollisionManager.h>
#include <IParticleSystemSceneNode.h>

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

namespace irr
{
namespace scene
{

u32 BoidSceneNode::boidID = 0;

BoidSceneNode::BoidSceneNode(
	IMeshBuffer* const shape,
	const core::vector3df& position, const irr::f32 borders[4], const f32 mimimumAboveGround, ISceneManager* const mgr) :
	ISceneNode(mgr->getRootSceneNode(), mgr, boidID, position),
	shape(shape),
	perching(false), perchTimer(0.0f), dontPerchTimer(0.0f),
	mimimumAboveGround(mimimumAboveGround),

	firstUpdate(true), lastAnimationTime(0), forward(true)
{
#ifdef _DEBUG
	setDebugName("BoidSceneNode");
#endif


	shape->grab();


	this->material.TextureLayer[0].Texture = SceneManager->getVideoDriver()->getTexture("media/images/temporal-wake.jpg");
	this->material.Shininess = 128.0f;
	this->material.EmissiveColor.set(255, 0, 0, 255);
	this->material.SpecularColor.set(255, 255, 255, 255);
	this->material.NormalizeNormals = true;


	//copy borders
	memcpy(this->borders, borders, sizeof(irr::f32)*4);

	//init ground ray
	this->groundRay.start = this->RelativeTranslation;
	this->groundRay.end = this->RelativeTranslation - irr::core::vector3df(0.0f, mimimumAboveGround, 0.0f);



	//store normals
	core::array<core::line3df> normals;

	core::vector3df normalizedNormal;
	static const f32 DebugNormalLength = 10.0f;


	const u32 vSize = video::getVertexPitchFromType(shape->getVertexType());
	const video::S3DVertex* v = (const video::S3DVertex*)shape->getVertices();
	const bool normalize = shape->getMaterial().NormalizeNormals;

	const u32 vCount = shape->getVertexCount();
	for (u32 i = 0; i != vCount; ++i)
	{
		normalizedNormal = v->Normal;
		if (normalize)
			normalizedNormal.normalize();

		normals.push_back(core::line3df(v->Pos, v->Pos + (normalizedNormal*DebugNormalLength)));

		v = (const video::S3DVertex*)((u8*)v + vSize);
	}


	//make vertex primitive list

	const u32 numNormals = normals.size();

	//this is for a line list with 2 lines per normal for rendering velocity and the ground ray
	this->numIndices = numNormals*2;
	this->numVertices = numNormals*2;

	// create an index buffer for rendering line segments
	this->indices = new u16[this->numIndices];
	for (u32 i = 0; i < this->numIndices; ++i)
		this->indices[i] = i;

	// create a vertex buffer for all of the line segments
	this->normalsVertices = new video::S3DVertex[this->numVertices];

	// add vertices for each normal
	u32 vIndex = 0;
	for (u32 n = 0; n < numNormals; ++n)
	{
		const core::line3df& normal = normals[n];

		//normal
		this->normalsVertices[vIndex].Pos = normal.start;
		this->normalsVertices[vIndex].Color.set(255, 255, 255, 0);
		this->normalsVertices[vIndex + 1].Pos = normal.end;
		this->normalsVertices[vIndex + 1].Color.set(255, 128, 128, 0);

		vIndex += 2;
	}


	this->normalsMaterial.Lighting = false;

	++boidID;
}

BoidSceneNode::~BoidSceneNode()
{
	this->shape->drop();
	delete[] this->normalsVertices;
	delete[] this->indices;
}

const core::aabbox3d<float>& BoidSceneNode::getBoundingBox() const
{
	return shape->getBoundingBox();
}

void BoidSceneNode::OnAnimate(u32 timeMs)
{
	if (firstUpdate)
	{
		this->lastAnimationTime = timeMs;
		firstUpdate = false;
	}

	const f32 elapsed = (timeMs - this->lastAnimationTime)*0.001;
	this->lastAnimationTime = timeMs;


	if (this->perching)
	{

		if (this->forward)
		{
			this->RelativeScale += irr::core::vector3df(1.0f, 1.0f, 1.0f)*elapsed;

			if (this->RelativeScale.X > 2.0f)
				this->forward = false;
		}
		else
		{
			this->RelativeScale -= irr::core::vector3df(1.0f, 1.0f, 1.0f)*elapsed;

			if (this->RelativeScale.X < 1.0f)
				this->forward = true;
		}

	}

	ISceneNode::OnAnimate(timeMs);
}

void BoidSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

void BoidSceneNode::render()
{
	video::IVideoDriver* const driver = SceneManager->getVideoDriver();
	driver->setTransform(video::ETS_WORLD, this->AbsoluteTransformation);

	//draw shape meshbuffer
	driver->setMaterial(this->material);
	driver->drawMeshBuffer(this->shape);

//	driver->drawIndexedTriangleList((video::S3DVertex*)shape->getVertices(), 4, shape->getIndices(), 4);

	//draw normals
	if (DebugDataVisible & scene::EDS_NORMALS)
	{
		driver->setMaterial(this->normalsMaterial);
		driver->drawVertexPrimitiveList(this->normalsVertices, this->numVertices, this->indices, this->numIndices/2,
			irr::video::EVT_STANDARD, irr::scene::EPT_LINES, irr::video::EIT_16BIT);
	}

	//draw oabb
	if (DebugDataVisible & scene::EDS_BBOX)
	{
		driver->setMaterial(this->normalsMaterial);
		driver->draw3DBox(shape->getBoundingBox(), video::SColor(255,255,255,255));
	}
}

u32 BoidSceneNode::getMaterialCount() const
{
	return 1;
}

video::SMaterial& BoidSceneNode::getMaterial(u32 i)
{
	return this->material;
}

const core::vector3df gIrrGetLookAtRotationDegreesLH(const core::vector3df& from, const core::vector3df& target)
{
	core::matrix4 m, m2;
	m.buildCameraLookAtMatrixLH(from, target, core::vector3df(0.0f, 1.0f, 0.0f));

	if (m.getInverse(m2))
		return m2.getRotationDegrees();
	else
	{
		if ((target - from).Y < 0.0f)
			return core::vector3df(90.0f, 0.0f, 0.0f);
		else
			return core::vector3df(-90.0f, 0.0f, 0.0f);
	}
}


void BoidSceneNode::applyRules(
		scene::ITriangleSelector* const selector,
		const core::array<BoidSceneNode*>& boids,
		const f32 distanceToOtherBoids,
		const f32 seekCenterOfMass,
		const f32 matchVelocity,
		const core::vector3df& target,
		const f32 tendencyTowardsPlace,
		const f32 tendencyAvoidPlace,
		const bool scatterFlock,
		const f32 scatterFlockModifier,
		const f32 deltaTime,
		const f32 speedLimit
#ifdef _SOUND
, irrklang::ISoundEngine* const soundEngine, const bool soundEnabled
#endif
)
{

	//check status

	if (this->perching)
	{
		this->perchTimer -= deltaTime;

		if (this->perchTimer > 0.0f)
			return; //dont apply the rules if boid is still perching
		else
			this->stopPerching();
	}

	else
	{
		if (this->dontPerchTimer > 0.0f)
		{
			this->dontPerchTimer -= deltaTime;

			if (this->dontPerchTimer <= 0.0f)
			{
				//reset color
				this->material.EmissiveColor.set(255, 0, 0, 255);

				//show normals
				this->DebugDataVisible &= (~irr::scene::EDS_NORMALS);
			}
		}
	}


	//rules by Reynolds (seperation, alignment, steer)
	const core::vector3df& reynolds = this->doReynolds(boids, distanceToOtherBoids, seekCenterOfMass, matchVelocity, scatterFlock, scatterFlockModifier);

	//tendency towards a particular place
	const core::vector3df& seek = scatterFlock ? irr::core::vector3df(0.0f, 0.0f, 0.0f) : this->seekTarget(target, tendencyTowardsPlace);

	//check for borders and ground
	const core::vector3df& avoid = this->bindPosition(selector, tendencyTowardsPlace, tendencyAvoidPlace);

	//bindPosition can make the boid perch, so check again
	if (this->perching)
		return;

	//set new velocity based upon rules
	this->velocity += reynolds + seek + avoid;

	this->limitSpeed(speedLimit);

	//set new position based upon rules and elapsed time
	this->RelativeTranslation += this->velocity*deltaTime;

	//turn boid
	this->RelativeRotation = gIrrGetLookAtRotationDegreesLH(this->RelativeTranslation, this->RelativeTranslation + this->velocity);
}

void BoidSceneNode::startPerching(const core::vector3df& outCollisionPoint)
{
	this->perching = true;

	//land on ground
	this->RelativeTranslation.Y = outCollisionPoint.Y + this->shape->getBoundingBox().getExtent().Y/2;

	//set velocity to 0
	this->velocity.set(0.0f, 0.0f, 0.0f);

	//decide how long to stay
	const irr::u32 perchTime = (rand()%6) + 3;
	this->perchTimer = perchTime;

	//change color
	this->material.EmissiveColor.set(255, 255, 0, 0);

	this->groundRay.end = this->RelativeTranslation;


	//create a particle system
	IParticleSystemSceneNode* const ps = this->SceneManager->addParticleSystemSceneNode(false, 0, -1, outCollisionPoint);
	ps->setMaterialFlag(video::EMF_LIGHTING, false);
	ps->setMaterialTexture(0, this->SceneManager->getVideoDriver()->getTexture("media/images/particle.bmp"));
	ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
	ps->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
	ps->setScale(core::vector3df(50.0f, 50.0f, 50.0f));

	IParticleEmitter* const em = ps->createPointEmitter(
			core::vector3df(0.0f, 0.003f, 0.0f),
			500,600,
			irr::video::SColor(0, 20, 20, 20), video::SColor(0, 70, 70, 70),
			900,1800,
			45);
	em->setMinStartSize(core::dimension2d<f32>(10.0f, 10.0f));
	em->setMaxStartSize(core::dimension2d<f32>(40.0f, 40.0f));
	ps->setEmitter(em);
	em->drop();


	//create particle affectors
	IParticleAffector* paf = 0;

	paf = ps->createFadeOutParticleAffector(video::SColor(0,0,0,0), 500);
	ps->addAffector(paf);
	paf->drop();

	paf = ps->createGravityAffector(core::vector3df(0.0f, 0.006f, 0.0f), 2000);
	ps->addAffector(paf);
	paf->drop();


	//create deletion animator for particle system
	ISceneNodeAnimator* const anim = this->SceneManager->createDeleteAnimator(perchTime*1000);
	ps->addAnimator(anim);
	anim->drop();

	/*
#ifdef _SOUND
	//play landing sound
	if (soundEngine && soundEnabled)
	{
		irrklang::ISound* const snd = soundEngine->play3D("media/sounds/mechanical_1.wav", this->RelativeTranslation, false, true);
		if (snd)
		{
			snd->setMinDistance(800.0f);
			snd->setIsPaused(false);
			snd->drop();
		}
	}
#endif
	 */
}

void BoidSceneNode::stopPerching()
{
	this->perching = false;

	this->forward = true; //reset scaling

	//reset scale to normal scale
	this->RelativeScale = core::vector3df(1.0f, 1.0f, 1.0f);

	//a little help with takeoff
	this->RelativeTranslation += core::vector3df(0.0f, this->shape->getBoundingBox().getExtent().Y/2, 0.0f);

	//change color
	this->material.EmissiveColor.set(255, 255, 255, 0);

	//reset perchTimer
	this->perchTimer = 0.0f;

	//don't perch again for 5 seconds
	this->dontPerchTimer = 5.0f;

	//add a particle system
	irr::scene::IParticleSystemSceneNode* const ps = this->SceneManager->addParticleSystemSceneNode(false, this);
	ps->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	ps->setMaterialTexture(0, this->SceneManager->getVideoDriver()->getTexture("media/images/particle.bmp"));
	ps->setMaterialType(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);
	ps->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
	ps->setScale(irr::core::vector3df(50.0f, 50.0f, 50.0f));

	irr::scene::IParticleEmitter* const em = ps->createPointEmitter(
			irr::core::vector3df(0.0f, 0.003f, 0.0f),
			50,100,
			irr::video::SColor(0, 255, 255, 0), irr::video::SColor(0, 128, 128, 0),
			200, 500,
			0);
	em->setMinStartSize(irr::core::dimension2d<irr::f32>(5.0f, 5.0f));
	em->setMaxStartSize(irr::core::dimension2d<irr::f32>(30.0f, 30.0f));
	ps->setEmitter(em);
	em->drop();

	//create particle affector
	irr::scene::IParticleAffector* const paf = ps->createFadeOutParticleAffector(irr::video::SColor(0,0,0,0), 200);
	ps->addAffector(paf);
	paf->drop();

	//create deletion animator for particle system
	irr::scene::ISceneNodeAnimator* const anim = this->SceneManager->createDeleteAnimator(this->dontPerchTimer*1000);
	ps->addAnimator(anim);
	anim->drop();

	this->DebugDataVisible |= irr::scene::EDS_NORMALS;
}


bool BoidSceneNode::isInNeighborhood(const BoidSceneNode* const other, const f32 radius) const
{
	//simple sphere test

	const core::vector3df relPos = this->RelativeTranslation - other->RelativeTranslation;
	const f32 dist = relPos.X*relPos.X + relPos.Y*relPos.Y + relPos.Z*relPos.Z;
	const f32 minDist = radius + radius;

	return dist <= minDist*minDist;
}

const core::vector3df BoidSceneNode::doReynolds(
		const core::array<BoidSceneNode*>& boids,
		const f32 distanceToOtherBoids,
		const f32 seekCenterOfMass,
		const f32 matchVelocity,
		const bool scatterFlock,
		const f32 scatterFlockModifier) const
{
	core::vector3df rule_1; //Boids try to fly towards the centre of mass of neighbouring boids.
	core::vector3df rule_2; //Boids try to keep a small distance away from other objects (including other boids).
	core::vector3df rule_3; //Boids try to match velocity with near boids.

	const u32 numBoids = boids.size();
	u32 numIgnoreBoids = 0;


	//what's larger: radius or distanceToOtherBoids?
	const irr::f32 radius = this->shape->getBoundingBox().getExtent().Y/2;
	const u32 minDistance = radius < distanceToOtherBoids ? distanceToOtherBoids : radius;

	const BoidSceneNode* otherBoid = 0;
	for (u32 other = 0; other < numBoids; ++other)
	{
		otherBoid = boids[other];

		if (*otherBoid != *this)
		{

			// don't take boid into account which are not in our vicinity
			// don't take perching boids into account
			if (!this->isInNeighborhood(otherBoid, radius*5) || otherBoid->perching)
			{
				++numIgnoreBoids;
				continue;
			}

			rule_1 += otherBoid->RelativeTranslation;

			//rule 2
			if (fabs(otherBoid->RelativeTranslation.X - this->RelativeTranslation.X) < minDistance)
			{
				rule_2.X -= otherBoid->RelativeTranslation.X - this->RelativeTranslation.X;
			}
			if (fabs(otherBoid->RelativeTranslation.Y - this->RelativeTranslation.Y) < minDistance)
			{
				rule_2.Y -= otherBoid->RelativeTranslation.Y - this->RelativeTranslation.Y;
			}
			if (fabs(otherBoid->RelativeTranslation.Z - this->RelativeTranslation.Z) < minDistance)
			{
				rule_2.Z -= otherBoid->RelativeTranslation.Z - this->RelativeTranslation.Z;
			}

			rule_3 += otherBoid->velocity;
		}
	}

	const irr::u32 numBoidsNotPerchingExceptThis = numBoids - numIgnoreBoids - 1;
	if (numBoidsNotPerchingExceptThis > 0)
	{
		rule_1 /= numBoidsNotPerchingExceptThis;
		rule_1 = (rule_1 - this->RelativeTranslation)/seekCenterOfMass;

		rule_3 /= (numBoidsNotPerchingExceptThis);
		rule_3 = (rule_3 - this->velocity)/matchVelocity;
	}

	return (scatterFlock ? -scatterFlockModifier*rule_1 : rule_1) + rule_2 + rule_3;
}

const core::vector3df BoidSceneNode::seekTarget(const core::vector3df& target, const f32 tendencyTowardsPlace) const
{
	//Especially for distant goals, one may want to limit the magnitude of the returned vector.
	return core::vector3df((target - this->RelativeTranslation)/tendencyTowardsPlace);
}

const core::vector3df BoidSceneNode::bindPosition(scene::ITriangleSelector* const selector, const f32 tendencyTowardsPlace, const f32 tendencyAvoidPlace)
{

	core::vector3df aV;

	//avoid x and z borders of the terrain
	if (this->RelativeTranslation.X < this->borders[0])
		aV.X = tendencyTowardsPlace;
	else if (this->RelativeTranslation.X > this->borders[1])
		aV.X = -tendencyTowardsPlace;

	if (this->RelativeTranslation.Z < this->borders[2])
		aV.Z = tendencyTowardsPlace;
	else if (this->RelativeTranslation.Z > this->borders[3])
		aV.Z = -tendencyTowardsPlace;

	//ckeck ground
	this->groundRay = core::line3d<f32>(
		this->RelativeTranslation, this->RelativeTranslation - core::vector3df(0.0f, this->mimimumAboveGround, 0.0f));

	//cast ray downwards to see if boid touches the terrain
	core::vector3df outCollisionPoint;
	core::triangle3df outTriangle;
	const ISceneNode* node = 0;
	if (this->SceneManager->getSceneCollisionManager()->getCollisionPoint(this->groundRay, selector, outCollisionPoint, outTriangle, node))
	{
		if (outCollisionPoint.Y >= this->RelativeTranslation.Y - this->shape->getBoundingBox().getExtent().Y/2)
		{
			if (this->dontPerchTimer > 0.0f)
			{
				//we were perching before, so use the avoid vector to stay away from the ground
				aV += -tendencyAvoidPlace*this->seekTarget(outCollisionPoint, tendencyTowardsPlace);
			}
			else
				this->startPerching(outCollisionPoint);
		}
		else
		{
			aV += -tendencyAvoidPlace*this->seekTarget(outCollisionPoint, tendencyTowardsPlace);
		}
	}

	return aV;
}

void BoidSceneNode::limitSpeed(const f32 speedLimit)
{
	//Limiting the speed. It is a good idea to limit the magnitude of the boids' velocities,
	//this way they don't go too fast. Without such limitations, their speed will actually
	//fluctuate with a flocking-like tendency, and it is possible for them to momentarily go very fast.
	const f32 absX = fabs(this->velocity.X);
	const f32 absY = fabs(this->velocity.Y);
	const f32 absZ = fabs(this->velocity.Z);

	if(absX > speedLimit)
		this->velocity.X = (this->velocity.X/absX)*speedLimit;
	if(absY > speedLimit)
		this->velocity.Y = (this->velocity.Y/absY)*speedLimit;
	if(absZ > speedLimit)
		this->velocity.Z = (this->velocity.Z/absZ)*speedLimit;
}

}
}
