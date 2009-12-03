// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "Demo.h"
#include <IrrlichtDevice.h>
#include <SMesh.h>
#include <SMeshBuffer.h>
#include <ISceneManager.h>
#include <IGUIEnvironment.h>

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

#include "MersenneTwister.h"

Demo::Demo(Configuration* const configuration, irr::IrrlichtDevice* const device
#ifdef _SOUND
		, irrklang::ISoundEngine* const soundEngine
#endif
) :
StateManager<Demo, irr::SEvent>(this),

configuration(configuration), device(device),
#ifdef _SOUND
soundEngine(soundEngine),
#endif
sceneManager(device->getSceneManager()),
videoDriver(device->getVideoDriver()),
guiEnvironment(device->getGUIEnvironment()),

font(0),

timer(device->getTimer()),
then(timer->getTime()),
now(0),
elapsed(0.0f),

rng(new MTRand(time(NULL)))
{
	//grab pointers to engines
	device->grab();
#ifdef _SOUND
	if (soundEngine)
		soundEngine->grab();
#endif

	//tell irrlicht that we are the event receiver
	this->device->setEventReceiver(this);

	//setup default demo font
	this->setFont("media/font/fontlucida.png");

	//setup material for debugging
	this->debugMat.Lighting = false;
}

Demo::~Demo()
{
	//release engine pointers
	this->device->drop();
#ifdef _SOUND
	if (this->soundEngine)
		this->soundEngine->drop();
#endif

	delete this->rng;
}

void Demo::addGameState(const char* const name, State<Demo, irr::SEvent>* const state)
{
	this->gameStates.push_back(SState(name, state));
}

State<Demo, irr::SEvent>* const Demo::findGameState(const char* const name)
{
	const irr::s32 f = this->gameStates.binary_search(SState(name));
	if (f != -1)
		return this->gameStates[f].state;

	return 0;
}

void Demo::setFont(const irr::c8* const filename)
{
	this->font = this->guiEnvironment->getFont(filename);
	if (this->font)
		this->guiEnvironment->getSkin()->setFont(this->font);
	else
	{
		this->device->getLogger()->log("Warning: Couldn't set font. Using default.", irr::ELL_WARNING);
		this->font = this->guiEnvironment->getBuiltInFont();
	}
}

void Demo::takeScreenshot() const
		{
	//get image from the last rendered frame
	irr::video::IImage* const image = this->videoDriver->createScreenShot();

	if (image) //should always be true, but you never know. ;)
	{
		//construct a filename, consisting of local time and file extension
		irr::c8 filename[64];
		snprintf(filename, 64, "capture/screenshot_%u.png", this->timer->getRealTime());

		//write screenshot to file
		if (!this->videoDriver->writeImageToFile(image, filename))
			this->device->getLogger()->log(L"Failed to take screenshot. Maybe you need to create the capture folder.", irr::ELL_WARNING);

		//Don't forget to drop image since we don't need it anymore.
		image->drop();
	}
		}

void Demo::rttCallBack()
{
	this->currentState->rttCallback(this);
}

void Demo::drawCallBack()
{
	this->videoDriver->setMaterial(this->debugMat);
	this->videoDriver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
	this->currentState->drawCallback(this);
}

void Demo::setTextureFiltering(const bool bilinear, const bool trilinear, const irr::u16 anisotropic) const
		{
	// find all nodes
	irr::core::array<irr::scene::ISceneNode*> nodes;
	this->sceneManager->getSceneNodesFromType(irr::scene::ESNT_ANY, nodes);

	const irr::u32 numNodes = nodes.size();

	for (irr::u32 n = 0; n < numNodes; ++n)
	{
		irr::scene::ISceneNode* const node = nodes[n];

		//		node->setAutomaticCulling(irr::scene::EAC_OFF);


		//		const irr::scene::ESCENE_NODE_TYPE& type = node->getType();
		//		const irr::s32 id = node->getID();
		//		const irr::c8* name = node->getName();

		const irr::u32 numMaterial = node->getMaterialCount();
		for (irr::u32 m = 0; m < numMaterial; ++m)
		{
			irr::video::SMaterial& mat = node->getMaterial(m);

			for (irr::u32 t = 0; t < irr::video::MATERIAL_MAX_TEXTURES; ++t)
			{
				mat.TextureLayer[t].BilinearFilter = bilinear;
				mat.TextureLayer[t].TrilinearFilter = trilinear;
				mat.TextureLayer[t].AnisotropicFilter = anisotropic;
			}
		}

	}
		}


irr::scene::IMesh* const Demo::createBoidMesh(const irr::f32 size,
	const irr::video::SColor& headColor, const irr::video::SColor& bodyColor) const
{
	irr::scene::SMeshBuffer* mb = new irr::scene::SMeshBuffer;
	if (!mb)
		return 0;
/*     .
	  /|\
     / | \
    / / \ \
   .-------.
*/

	mb->Vertices.set_used(4);
	mb->Indices.set_used(12);

	mb->Vertices[0].Pos.set(0.0f, 0.0f, 0.5f*size);
	mb->Vertices[0].Normal.set(0.0f, 1.0f, 0.0f);
	mb->Vertices[0].Color.set(headColor.color);
	mb->Vertices[0].TCoords.set(0.0f, 1.0f);

	mb->Vertices[1].Pos.set(0.25f*size, 0.0f, -0.5f*size);
	mb->Vertices[1].Normal.set(0.0f, 1.0f, 0.0f);
	mb->Vertices[1].Color.set(bodyColor.color);
	mb->Vertices[1].TCoords.set(1.0f, 1.0f);

	mb->Vertices[2].Pos.set(-0.25f*size,0.0f, -0.5f*size);
	mb->Vertices[2].Normal.set(0.0f, 1.0f, 0.0f);
	mb->Vertices[2].Color.set(bodyColor.color);
	mb->Vertices[2].TCoords.set(0.0f, 0.0f);

	mb->Vertices[3].Pos.set(0.0f, 0.5f*size, -0.25f*size);
	mb->Vertices[3].Normal.set(0.0f, 1.0f, 0.0f);
	mb->Vertices[3].Color.set(bodyColor.color);
	mb->Vertices[3].TCoords.set(1.0f, 0.0f);


	// left face
	mb->Indices[0] = 2;
	mb->Indices[1] = 0;
	mb->Indices[2] = 3;

	// bottom face
	mb->Indices[3] = 2;
	mb->Indices[4] = 1;
	mb->Indices[5] = 0;

	// right face
	mb->Indices[6] = 0;
	mb->Indices[7] = 1;
	mb->Indices[8] = 3;

	// back face
	mb->Indices[9] = 1;
	mb->Indices[10] = 2;
	mb->Indices[11] = 3;

	mb->recalculateBoundingBox();


	//... more to be added


	irr::scene::SMesh* mesh = new irr::scene::SMesh;
	mesh->addMeshBuffer(mb);
	mb->drop();

	mesh->recalculateBoundingBox();
	mesh->setHardwareMappingHint(irr::scene::EHM_STATIC);

	return mesh;
}


//driver->drawIndexedTriangleFan(&mb->Vertices[0], mb->Vertices.size(), &mb->Indices[0], mb->Indices.size() - 2);
irr::scene::SMeshBuffer* createCircleFan(irr::u32 verts, irr::f32 radius, const irr::video::SColor& color)
{
	if (verts < 3)
		verts = 3;
	else if (65534 < verts)
		verts = 65534;

	irr::scene::SMeshBuffer* mb = new irr::scene::SMeshBuffer;
	if (!mb)
		return 0;

	irr::video::S3DVertex vertex;
	vertex.Normal.set(0, 1, 0);
	vertex.Color = color;

	// origin vertex
	vertex.Pos.set(0, 0, 0);
	vertex.TCoords.set(.5f, .5f);
	mb->Vertices.push_back(vertex);
	mb->Indices.push_back(0);

	for (irr::s32 n = verts; 0 <= n; --n)
	{
		const irr::f32 x = (irr::core::PI*2.f)*n/verts;

		const irr::f32 dx = sinf(x);
		const irr::f32 dz = cosf(x);

		// vertices are always in the xz plane
		vertex.Pos.set(dx*radius, 0, dz*radius);

		// scale tcoords from -1,1 to 0,1
		vertex.TCoords.set((dx + 1.f) * .5f, (dz + 1.f) * .5f);

		mb->Vertices.push_back(vertex);
		mb->Indices.push_back(n + 1);
	}

	return mb;
}
