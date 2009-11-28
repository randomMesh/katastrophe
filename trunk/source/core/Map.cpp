// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "Map.h"

#include <ISceneManager.h>
#include <IMeshCache.h>
#include <IVideoDriver.h>
#include <IMetaTriangleSelector.h>
#include <IParticleSystemSceneNode.h>
#include <ITerrainSceneNode.h>
#include <ICameraSceneNode.h>
#include <SKeyMap.h>
#include <IBillboardSceneNode.h>
#include <IVolumeLightSceneNode.h>
#include <ILightSceneNode.h>
#include <ISceneCollisionManager.h>
#include <ISceneNodeAnimatorCollisionResponse.h>

#include "Demo.h"
#include "Configuration.h"
#include "Flock.h"
#include "../scene/grass/CGrassGeneratorNode.h"
#include "../scene/grass/CWindGenerator.h"
#include "../scene/TeleporterSceneNode.h"
#include "../scene/Forest.h"
#include "../scene/lensflare/CLensFlareSceneNode.h"
#include "../scene/CBeamSceneNode.h"
#include "../scene/BoidSceneNode.h"

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

#include <IrrlichtDevice.h>

Map::Map(Demo* const demo) :
	demo(demo), flock(0),
	collmgr(demo->getSceneManager()->getSceneCollisionManager()), selector(0), anim(0), ps(0),
	terrain(0), forest(0), grassGeneratorNode(0), cursor(0), crosshair(0)
#ifdef _SOUND
	, wind(0)
#endif
{

}

Map::~Map()
{
	this->clear();
}

bool Map::load(const irr::core::stringc& fileName)
{
	this->clear();

	if (fileName == "")
	{
		this->loadDefault();
		return true;
	}
	else
	{
		//TODO: load map from file
		return false;
	}
}

void Map::loadDefault()
{
	const Configuration* const config = this->demo->getConfiguration();
	irr::scene::ISceneManager* const smgr = this->demo->getSceneManager();
	irr::video::IVideoDriver* const driver = this->demo->getVideoDriver();


	//create master selector
	this->selector = smgr->createMetaTriangleSelector();

	//reset ambient light from previous state
	smgr->setAmbientLight(irr::video::SColorf(0.0f, 0.0f, 0.0f));



	//setup 'player'
	this->playerStartPosition = irr::core::vector3df(7950.0f, 550.0f, 6050.0f);
	this->playerStartTarget = irr::core::vector3df(-1.0f, 550.0f, 6050.0f);

	irr::scene::IMesh* const startMesh = smgr->getGeometryCreator()->createCylinderMesh(150.0f, 100.0f, 5);
	startMesh->setHardwareMappingHint(irr::scene::EHM_STATIC);
	irr::scene::IMeshSceneNode* const startNode = smgr->addMeshSceneNode(startMesh);
	startNode->setPosition(this->playerStartPosition - irr::core::vector3df(0.0f, 360.0f, 0.0f));
	startNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	startNode->setMaterialTexture(0, driver->getTexture("media/images/checkerboard.png"));
	startMesh->drop();

	irr::scene::ITriangleSelector* const startSelector = smgr->createTriangleSelector(startMesh, startNode);
	startNode->setTriangleSelector(startSelector);
	this->selector->addTriangleSelector(startSelector);
	startSelector->drop();



	irr::scene::IParticleSystemSceneNode* const startEmitter = smgr->addParticleSystemSceneNode(false, startNode);
	startEmitter->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	startEmitter->setMaterialTexture(0, driver->getTexture("media/images/button.png"));
	startEmitter->setMaterialType(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);
	irr::scene::IParticleRingEmitter* const re = startEmitter->createRingEmitter(
			irr::core::vector3df(0.0f, startNode->getBoundingBox().getExtent().Y, 0.0f), 100.0f, 10.0f, irr::core::vector3df(0.0f, 0.1f, 0.0f),
			100,
			500,
			irr::video::SColor(0, 0, 0 , 255),
			irr::video::SColor(0, 255, 255, 255),
			1500, 3000, 0,
			irr::core::dimension2df(0.5f, 2.0f),
			irr::core::dimension2df(0.5f, 2.0f));
	startEmitter->setEmitter(re);
	re->drop();

	irr::scene::IParticleFadeOutAffector* const spaf = startEmitter->createFadeOutParticleAffector();
	startEmitter->addAffector(spaf);
	spaf->drop();


	//add a camera
	irr::SKeyMap keyMap[10];
	keyMap[0].Action = irr::EKA_MOVE_FORWARD;
	keyMap[0].KeyCode = irr::KEY_UP;
	keyMap[1].Action = irr::EKA_MOVE_FORWARD;
	keyMap[1].KeyCode = irr::KEY_KEY_W;
	keyMap[2].Action = irr::EKA_MOVE_BACKWARD;
	keyMap[2].KeyCode = irr::KEY_DOWN;
	keyMap[3].Action = irr::EKA_MOVE_BACKWARD;
	keyMap[3].KeyCode = irr::KEY_KEY_S;
	keyMap[4].Action = irr::EKA_STRAFE_LEFT;
	keyMap[4].KeyCode = irr::KEY_LEFT;
	keyMap[5].Action = irr::EKA_STRAFE_LEFT;
	keyMap[5].KeyCode = irr::KEY_KEY_A;
	keyMap[6].Action = irr::EKA_STRAFE_RIGHT;
	keyMap[6].KeyCode = irr::KEY_RIGHT;
	keyMap[7].Action = irr::EKA_STRAFE_RIGHT;
	keyMap[7].KeyCode = irr::KEY_KEY_D;
	keyMap[8].Action = irr::EKA_JUMP_UP;
	keyMap[8].KeyCode = irr::KEY_SPACE;
	keyMap[9].Action = irr::EKA_CROUCH;
	keyMap[9].KeyCode = irr::KEY_KEY_C;

	irr::scene::ICameraSceneNode* const camera = smgr->addCameraSceneNodeFPS(0, config->getCameraRotateSpeed(), config->getCameraMoveSpeed(),
			-1, keyMap, 10, true, config->getCameraJumpSpeed(), config->isInvertMouse());



	//add animated skybox
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	irr::scene::ISceneNode* const skybox = smgr->addSkyBoxSceneNode(
			driver->getTexture("media/images/skybox/starfield/starfield_up.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_dn.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_lf.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_rt.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_ft.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_bk.png"));
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);

	irr::scene::ISceneNodeAnimator* const skyAnim = smgr->createRotationAnimator(irr::core::vector3df(0.0f, 0.006f, 0.0f));
	skybox->addAnimator(skyAnim);
	skyAnim->drop();



	//add terrain scene node
	this->terrain = smgr->addTerrainSceneNode(
			"media/images/terrain/terrain-heightmap.bmp",
			0,											// parent node
			-1,											// node id
			irr::core::vector3df(0.0f, 0.0f, 0.0f),		// position
			irr::core::vector3df(0.0f, 0.0f, 0.0f),		// rotation
			irr::core::vector3df(40.0f, 2.0f, 40.0f),	// scale
			irr::video::SColor(255, 255, 255, 255),		// vertexColor
			16,											// maxLOD
			irr::scene::ETPS_17,						// patchSize
			4);											// smoothFactor
	this->terrain->setMaterialTexture(0, driver->getTexture("media/images/terrain/terrain-texture.jpg"));
	this->terrain->setMaterialTexture(1, driver->getTexture("media/images/terrain/detailmap3.jpg"));
	this->terrain->setMaterialType(irr::video::EMT_DETAIL_MAP);
	this->terrain->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	this->terrain->scaleTexture(1.0f, 50.0f);




	// create triangle selector for the terrain
	irr::scene::ITriangleSelector* const terrainSel = smgr->createTerrainTriangleSelector(this->terrain, 0);
	this->terrain->setTriangleSelector(terrainSel);
	this->selector->addTriangleSelector(terrainSel);
	terrainSel->drop();



	//create flock
	const irr::core::aabbox3df& tb = this->terrain->getBoundingBox();
	const irr::f32 borders[4] = { 0.0f, tb.getExtent().X, 0.0f, tb.getExtent().Z }; //Xmin, Xmax, Zmin, Zmax

	const irr::core::vector3df flockTarget(3000.0f, 700.0f, 4000.0f);

	this->flock = new Flock(this->demo, flockTarget, borders);


	//add boids to flock
	irr::scene::IMesh* const boidMesh = smgr->getGeometryCreator()->createSphereMesh(25.0f, 32, 32);

	for (irr::u32 i = 0; i < 20; ++i)
		this->flock->addBoid(boidMesh);

	boidMesh->drop();



	//create lens flare node
	irr::scene::CLensFlareSceneNode* const flare = new irr::scene::CLensFlareSceneNode(smgr->getRootSceneNode(), smgr);
	flare->setPosition(irr::core::vector3df(this->terrain->getTerrainCenter().X, 5500.0f, this->terrain->getTerrainCenter().Z));
	flare->getMaterial(0).setTexture(0, driver->getTexture("media/images/flares.jpg")); // attach the flare images texture, needs to be n quad sized textures from left to right
	flare->drop();



	// create wind
	irr::scene::CWindGenerator* const wind = new irr::scene::CWindGenerator;
	wind->setStrength(30.0f);
	wind->setRegularity(3.0f);


	//add grass
	const irr::video::IImage* const heightMap = driver->createImageFromFile("media/images/terrain/terrain-heightmap.bmp");
	const irr::video::IImage* const textureMap = driver->createImageFromFile("media/images/terrain/terrain-grasscol.bmp");
	const irr::video::IImage* const grassMap = driver->createImageFromFile("media/images/terrain/terrain-grassmap.png");

	this->grassGeneratorNode = new irr::scene::CGrassGeneratorNode(smgr);
	this->grassGeneratorNode->addGrassToTerrain(wind, this->terrain, heightMap, textureMap, grassMap);
	this->grassGeneratorNode->increaseDrawDistance(5000.0f);
//	this->grassGeneratorNode->increaseMaxDensity(2950.0f);
	this->grassGeneratorNode->setVisible(config->getShowGrass());


	heightMap->drop();
	textureMap->drop();
	grassMap->drop();

	wind->drop();


	this->crosshair = smgr->addBillboardSceneNode(
			camera, irr::core::dimension2df(4.0f, 4.0f), irr::core::vector3df(0.0f, 0.0f, 70.0f));
	this->crosshair->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
	this->crosshair->setMaterialTexture(0, driver->getTexture("media/images/crosshair/cross.bmp"));
	this->crosshair->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	this->crosshair->setMaterialFlag(irr::video::EMF_ZBUFFER, false);


	//position camera
	camera->setPosition(this->playerStartPosition);
	camera->setTarget(this->playerStartTarget);
	camera->setFarValue(12000.0f);
	camera->setFOV(60.0f*irr::core::DEGTORAD);
	//camera->setNearValue(0.5f);

	// create collision response animator and attach it to the camera
	this->anim = smgr->createCollisionResponseAnimator(this->selector,
			camera, irr::core::vector3df(60.0f, 100.0f, 60.0f),
			irr::core::vector3df(0.0f, -9.80665f, 0.0f), //gravity
			irr::core::vector3df(0.0f, 0.0f, 0.0f));
	camera->addAnimator(this->anim);


	// load textures for animation
	irr::core::array<irr::video::ITexture*> textures;
	irr::core::stringc tmp;
	for (irr::u32 i = 7; i > 0; --i)
	{
		tmp = "media/images/portal/portal";
		tmp += i;
		tmp += ".bmp";

		textures.push_back(driver->getTexture(tmp.c_str()));
	}

	// create texture animator
	irr::scene::ISceneNodeAnimator* const glow = smgr->createTextureAnimator(textures, 50);



	//add some kind of cursor
	this->cursor = smgr->addVolumeLightSceneNode(0, -1,
			32,										// Subdivisions on U axis
			32,										// Subdivisions on V axis
			irr::video::SColor(51, 0, 230, 180),	// foot color
			irr::video::SColor(0, 0, 0, 0));		// tail color
	this->cursor->setPosition(flockTarget);
	this->cursor->setScale(irr::core::vector3df(50.0f, 60.0f, 50.0f));
	this->cursor->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	this->cursor->addAnimator(glow);
	this->cursor->setMaterialType(irr::video::EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR);
	//this->cursor->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);




	//add teleporter

	//bounds of placeA
	const irr::core::aabbox3df placeA(
			irr::core::vector3df(1900.0f, 1600.0f, 3450.0f),
			irr::core::vector3df(2100.0f, 1800.0f, 3650.0f));

	//bounds of placeB
	const irr::core::aabbox3df placeB(
			irr::core::vector3df(3900.0f, 2100.0f, 6400.0f),
			irr::core::vector3df(4100.0f, 2300.0f, 6600.0f));

	//true if teleport from B to A is possible, too
	const bool viceversa = true;

	//use default target when teleported to place A
	const bool useLookDirA = false;
	const irr::core::vector3df& placeALookDir = irr::core::vector3df();

	//use custom target when teleported to place B
	const bool useLookDirB = true;
	const irr::core::vector3df& placeBLookDir = irr::core::vector3df(4000.0f, 2100.0f, -1.0f);

	irr::scene::TeleporterSceneNode* const teleporter = new irr::scene::TeleporterSceneNode(
			this->demo, placeA, placeB, viceversa, useLookDirA, useLookDirB, placeALookDir, placeBLookDir);
	this->teleporters.push_back(teleporter);
	//teleporter->setDebugDataVisible(irr::scene::EDS_BBOX); //show bounds of placeA and placeB

	//add scene nodes which are effected by the teleporter
	teleporter->addNodeToWatchList(camera);

	//add boids to teleporter watch list
	const irr::core::array<irr::scene::BoidSceneNode*>& boids = this->flock->getBoids();
	for(irr::u32 i = 0; i < boids.size(); ++i)
	{
		teleporter->addNodeToWatchList(boids[i]);
	}




	irr::core::array<irr::core::aabbox3df> exceptions; //places where not to place any trees
	exceptions.push_back(startNode->getTransformedBoundingBox()); //player start
	exceptions.push_back(placeA); //teleporters
	exceptions.push_back(placeB);

	//add trees
	this->forest = new Forest(this->demo->getDevice(), this->terrain, exceptions);
	//this->forest->randomlyPlaceVegetation(50);




	//add 'water'
	irr::scene::IAnimatedMesh* const waterMesh = smgr->addHillPlaneMesh("myHill",
			irr::core::dimension2d<irr::f32>(100.0f, 100.0f),	//tile size
			irr::core::dimension2d<irr::u32>(25, 25),			//tile count
			0, 0.0f,											//material, hill height
			irr::core::dimension2d<irr::f32>(0.0f, 0.0f),		//countHills
			irr::core::dimension2d<irr::f32>(6.0f, 6.0f));		//textureRepeatCount

	irr::scene::ISceneNode* const waterNode = smgr->addWaterSurfaceSceneNode(waterMesh->getMesh(0), 1.0f, 600.0f, 2.0f);
	waterNode->setPosition(irr::core::vector3df(8350.0f, 80.0f, 2600.0f));
	waterNode->setMaterialTexture(0, driver->getTexture("media/images/terrain/detailmap3.jpg"));
	waterNode->getMaterial(0).getTextureMatrix(0).setTextureScaleCenter(2.0f, 2.0f);
	waterNode->setMaterialTexture(1, driver->getTexture("media/images/water/water.jpg"));
	waterNode->getMaterial(1).getTextureMatrix(0).setTextureScaleCenter(2.0f, 2.0f);
	waterNode->setMaterialType(irr::video::EMT_REFLECTION_2_LAYER);
	waterNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);


	//add a particle system to the camera
	this->ps = smgr->addParticleSystemSceneNode(false, camera);
	this->ps->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	this->ps->setMaterialTexture(0, driver->getTexture("media/images/omg.png"));
	this->ps->setMaterialType(irr::video::EMT_TRANSPARENT_VERTEX_ALPHA);

	irr::scene::IParticleEmitter* const em = this->ps->createBoxEmitter(
			irr::core::aabbox3d<irr::f32>(-250.0f, -50.0f, -50.0f, 250.0f, 50.0f, 500.0f), // emitter size
			irr::core::vector3df(0.0f, 0.005f, 0.0f),	// initial direction
			20, 50,										// emit rate
			irr::video::SColor(0, 255, 255, 255),		// darkest color
			irr::video::SColor(0, 0, 0, 0),				// brightest color
			1000, 3000, 0,								// min and max age, angle
			irr::core::dimension2df(0.5f, 1.0f),		// min size
			irr::core::dimension2df(1.0f, 2.0f));		// max size
	this->ps->setEmitter(em);
	em->drop();

	irr::scene::IParticleAffector* paf = 0;

	paf = this->ps->createFadeOutParticleAffector();
	this->ps->addAffector(paf);
	paf->drop();

	paf = this->ps->createScaleParticleAffector(irr::core::dimension2df(4.0f, 2.0f));
	this->ps->addAffector(paf);
	paf->drop();



	//stairs
	irr::scene::IMeshSceneNode* stair = 0;
	irr::scene::ITriangleSelector* sel = 0;

	irr::video::SMaterial mat;
	mat.TextureLayer[0].Texture = driver->getTexture("media/images/temporal-wake.jpg");
	mat.EmissiveColor = irr::video::SColor(255, 255, 0, 0);
	mat.SpecularColor = irr::video::SColor(0, 0, 0, 0);
	mat.DiffuseColor =  irr::video::SColor(0, 0, 0, 0);
	mat.Shininess = 128.0f;
	mat.NormalizeNormals = true;

	for (irr::u32 i = 0; i < 10; ++i)
	{
		stair = smgr->addCubeSceneNode(120, 0, -1, irr::core::vector3df(2000.0f, 150.0f*i + 150, 150*i + 2000.0f));
		stair->getMesh()->setHardwareMappingHint(irr::scene::EHM_STATIC);
		stair->getMaterial(0) = mat;

		sel = smgr->createTriangleSelector(stair->getMesh(), stair);
		stair->setTriangleSelector(sel);
		this->selector->addTriangleSelector(sel);
		sel->drop();
	}


	//big cube platform
	irr::scene::IMeshSceneNode* const cube = smgr->addCubeSceneNode(500.0f, 0, -1, irr::core::vector3df(4000.0f, 1500, 6500.0f));
	cube->getMesh()->setHardwareMappingHint(irr::scene::EHM_STATIC);
	cube->getMaterial(0) = mat;

	sel = smgr->createTriangleSelector(cube->getMesh(), cube);
	cube->setTriangleSelector(sel);
	this->selector->addTriangleSelector(sel);
	sel->drop();


	//walkway
	irr::scene::IMeshSceneNode* const walkway = smgr->addCubeSceneNode(250.0f, 0, -1, irr::core::vector3df(4000.0f, 1500, 5500.0f));
	walkway->getMesh()->setHardwareMappingHint(irr::scene::EHM_STATIC);
	walkway->setScale(irr::core::vector3df(2.0f, 1.0f, 5.0f));
	walkway->getMaterial(0) = mat;

	sel = smgr->createTriangleSelector(walkway->getMesh(), walkway);
	walkway->setTriangleSelector(sel);
	this->selector->addTriangleSelector(sel);
	sel->drop();






	// add lights (one for each corner and the center of the terrain)
	irr::scene::ILightSceneNode* light[5];
	irr::scene::IBillboardSceneNode* lightBillboard[5];

	memset(light, 0, sizeof(irr::scene::ILightSceneNode*)*5);
	memset(lightBillboard, 0, sizeof(irr::scene::IBillboardSceneNode*)*5);

	irr::scene::ITriangleSelector* lightSel = 0;
	for (irr::u32 numLight = 0; numLight < 5; ++numLight)
	{
		light[numLight] = smgr->addLightSceneNode();
		light[numLight]->setRadius(3000.0f);

		lightBillboard[numLight] = smgr->addBillboardSceneNode(light[numLight], irr::core::dimension2d<irr::f32>(200.0f, 200.0f));
		lightBillboard[numLight]->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		lightBillboard[numLight]->setMaterialFlag(irr::video::EMF_FOG_ENABLE, false);
		lightBillboard[numLight]->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
		lightBillboard[numLight]->addAnimator(glow);

		lightSel = smgr->createTriangleSelectorFromBoundingBox(lightBillboard[numLight]);
		lightBillboard[numLight]->setTriangleSelector(lightSel);
		this->selector->addTriangleSelector(lightSel);
		lightSel->drop();

		smgr->addBillboardTextSceneNode(
				demo->getFont(), L"Light", lightBillboard[numLight],
				irr::core::dimension2df(120.0f, 100.0f),
				irr::core::vector3df(0.0f, 100.0f, 0.0f), -1,
				irr::video::SColor(255, 255, 0, 255), irr::video::SColor(255, 255, 255, 255));
	}

	glow->drop();


	static const irr::f32 lightHeight = 1000.0f; //above ground

	static irr::core::vector3df edges[8];
	this->terrain->getBoundingBox().getEdges(edges);
	const irr::core::vector3df& terrainCenter = this->terrain->getTerrainCenter();

	light[0]->setPosition(irr::core::vector3df(terrainCenter.X, this->terrain->getHeight(terrainCenter.X, terrainCenter.Z) + lightHeight, terrainCenter.Z));
	light[1]->setPosition(irr::core::vector3df(edges[0].X, this->terrain->getHeight(edges[0].X, edges[0].Z) + lightHeight, edges[0].Z));
	light[2]->setPosition(irr::core::vector3df(edges[2].X, this->terrain->getHeight(edges[2].X, edges[2].Z) + lightHeight, edges[2].Z));
	light[3]->setPosition(irr::core::vector3df(edges[4].X, this->terrain->getHeight(edges[4].X, edges[4].Z) + lightHeight, edges[4].Z));
	light[4]->setPosition(irr::core::vector3df(edges[6].X, this->terrain->getHeight(edges[6].X, edges[6].Z) + lightHeight, edges[6].Z));


	irr::video::ITexture* const tex = driver->getTexture("media/images/beam.png");
	if (tex)
	{
		irr::scene::IMesh* const mesh = smgr->getGeometryCreator()->createCylinderMesh(50.0f, lightHeight - 50.0f, 32);
		mesh->setHardwareMappingHint(irr::scene::EHM_STATIC);

		irr::scene::IMeshSceneNode* cylinder = 0;
		irr::scene::ITriangleSelector* sel = 0;
		for (irr::u32 numLight = 0; numLight < 5; ++numLight)
		{
			cylinder = smgr->addMeshSceneNode(mesh, 0, -1,
				light[numLight]->getPosition() - irr::core::vector3df(0.0f, lightHeight, 0.0f));
			cylinder->setMaterialTexture(0, tex);
			cylinder->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
			//cylinder->setDebugDataVisible(irr::scene::EDS_BBOX);

			sel = smgr->createTriangleSelector(mesh, cylinder);
			cylinder->setTriangleSelector(sel);
			this->selector->addTriangleSelector(sel);
			sel->drop();
		}

		mesh->drop();
	}


//	driver->getOverrideMaterial().Material.ColorMask = irr::video::ECP_RED;
//	driver->getOverrideMaterial().EnableFlags = irr::video::EMF_COLOR_MASK;
//	driver->getOverrideMaterial().EnablePasses = irr::scene::ESNRP_SKY_BOX | irr::scene::ESNRP_SOLID | irr::scene::ESNRP_TRANSPARENT | irr::scene::ESNRP_TRANSPARENT_EFFECT | irr::scene::ESNRP_SHADOW;
/*
	irr::scene::ISceneNode* root = smgr->getRootSceneNode();
	irr::core::list<irr::scene::ISceneNode*> children = root->getChildren();

	irr::core::list<irr::scene::ISceneNode*>::Iterator it = children.begin();
	const irr::core::list<irr::scene::ISceneNode*>::Iterator& end = children.end();

	for (; it != end; ++it)
	{
		//(*it)->setAutomaticCulling(irr::scene::EAC_FRUSTUM_SPHERE);
		(*it)->setMaterialFlag(irr::video::EMF_TRILINEAR_FILTER, true);
		(*it)->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, true);
		(*it)->setMaterialFlag(irr::video::EMF_COLOR_MASK, true);
	}
*/
}

void Map::clear()
{

	if (this->grassGeneratorNode)
	{
		this->grassGeneratorNode->remove(); //?
		this->grassGeneratorNode->drop();
		this->grassGeneratorNode = 0;
	}

	for (irr::u32 i = 0; i < this->teleporters.size(); ++i)
	{
		this->teleporters[i]->remove(); //?
		this->teleporters[i]->drop();
	}
	this->teleporters.clear();


	delete this->forest;
	this->forest = 0;

	delete this->flock;
	this->flock = 0;


	if (this->anim)
	{
		this->anim->drop();
		this->anim = 0;
	}

	if (this->selector)
	{
		this->selector->removeAllTriangleSelectors(); //?
		this->selector->drop();
		this->selector = 0;
	}

	this->demo->getSceneManager()->getMeshCache()->clear();
	this->demo->getSceneManager()->clear();

	this->terrain = 0;
	this->ps = 0;

#ifdef _SOUND
	stopSounds();
#endif
}

void Map::update(const bool freezeTarget, const irr::core::vector3df& camPos, const irr::core::vector3df& camTarget, const bool rightMouseButton)
{

	irr::core::vector3df intersection;
	irr::core::triangle3df tri;

	const irr::core::line3d<irr::f32> line(camPos, camPos + (camTarget - camPos).normalize()*12000.0f);

	const irr::scene::ISceneNode* node = 0;
	if (this->collmgr->getCollisionPoint(line, this->selector, intersection, tri, node))
	{
		if (!freezeTarget)
		{
			//update flock target and cursor
			this->flock->setTarget(intersection);
			this->cursor->setPosition(intersection);
		}

		/*
		if (node)
		{
			printf("Selected: %s\n", demo->getSceneManager()->getSceneNodeTypeName(node->getType()));

			if (node != this->terrain)
			{
				this->crosshair->setSize(irr::core::dimension2df(6.0f, 6.0f));
			}
			else
			{
				this->crosshair->setSize(irr::core::dimension2df(5.0f, 5.0f));
			}
		}
		 */
	}
	//	else
	//		this->crosshair->setSize(irr::core::dimension2df(5.0f, 5.0f));


	//update flock
	this->flock->update(this->selector, rightMouseButton);



	if (this->anim->collisionOccurred())
	{

	}
	else
	{
		//check 'player' position and teleport him back if his Y position is < -200.0
		if (camPos.Y < -200.0f)
		{
			irr::scene::ICameraSceneNode* const camera = demo->getSceneManager()->getActiveCamera();

			camera->setPosition(this->playerStartPosition);
			camera->updateAbsolutePosition(); // don't remove - see comment below
			// This only works if setPosition and updateAbsolutePosition are called first
			camera->setTarget(this->playerStartTarget);

			//in order to teleport a scene node with an attached collision response animator, we need to update its target
			this->anim->setTargetNode(camera);

#ifdef _SOUND
			irrklang::ISoundEngine* const soundEngine = this->demo->getSoundEngine();

			//play teleport sound
			if (soundEngine && this->demo->getConfiguration()->isSoundEnabled())
			{
				irrklang::ISound* snd = 0;
				snd = soundEngine->play3D("media/sounds/teleport.wav", this->playerStartPosition, false, true);
				if (snd)
				{
					snd->setMinDistance(500.0f);
					//	snd->setVolume(0.5f);
					snd->setIsPaused(false);
					snd->drop();
					snd = 0;
				}
			}
#endif
		}
	}
}

#ifdef _SOUND
void Map::startSounds()
{
	stopSounds();

	//existence of sound engine and sound config flag already have been checked
	this->wind = this->demo->getSoundEngine()->play2D("media/sounds/wind.ogg", true, false, true);
}

void Map::stopSounds()
{
	if (this->wind)
	{
		this->wind->stop();
		this->wind->drop();
		this->wind = 0;
	}
}
#endif

irr::scene::BoidSceneNode* const Map::addBoid()
{
	irr::scene::IMesh* const boidMesh = demo->getSceneManager()->getGeometryCreator()->createSphereMesh(25.0f, 32, 32);

	irr::scene::BoidSceneNode* const boid = this->flock->addBoid(boidMesh);
	boidMesh->drop();

	//add boid to all teleporters
	const irr::u32 numTeleporters = this->teleporters.size();
	for (irr::u32 i = 0; i < numTeleporters; ++i)
		this->teleporters[i]->addNodeToWatchList(boid);

	return boid;
}

bool Map::removeBoid(irr::scene::BoidSceneNode* const boid)
{
	//remove boid from all teleporters
	const irr::u32 numTeleporters = this->teleporters.size();
	for (irr::u32 i = 0; i < numTeleporters; ++i)
		this->teleporters[i]->removeNodeFromWatchList(boid);

	//remove boid from flock
	return this->flock->removeBoid(boid);
}

void Map::drawDebug() const
{
	const irr::core::array<irr::scene::BoidSceneNode*>& boids = this->flock->getBoids();
	const irr::u32 numBoids = boids.size();


	//this is for a line list with 2 line per boid for rendering velocity and the ground ray
	const irr::u32 n_indices = numBoids*4;
	const irr::u32 n_vertices = numBoids*4;

	// create an index buffer for rendering line segments
	irr::u16 indices[n_indices];
	for (irr::u32 i = 0; i < n_indices; ++i)
		indices[i] = i;

	// create a vertex buffer for all of the line segments
	irr::video::S3DVertex vertices[n_vertices];

	// add vertices for each boid
	static irr::f32 velocity[3];
	irr::u32 vIndex = 0;
	for (irr::u32 p = 0; p < numBoids; ++p)
	{
		//get velocity and ground ray
		memcpy(velocity, boids[p]->getVelocity(), sizeof(irr::f32)*3);
		const irr::core::vector3df& boidPos = boids[p]->getPosition();

		//line 1 (velocity)
		vertices[vIndex].Pos = boidPos;
		vertices[vIndex].Color.set(255, 0, 255, 0);
		vertices[vIndex + 1].Pos.set(boidPos + irr::core::vector3df(velocity[0], velocity[1], velocity[2]).normalize()*100.0f);
		vertices[vIndex + 1].Color.set(255, 255, 0, 0);

		//line 2 (ground ray)
		vertices[vIndex + 2].Pos = boidPos;
		vertices[vIndex + 2].Color.set(255, 0, 0, 255);
		vertices[vIndex + 3].Pos = boids[p]->getGroundRay().end;
		vertices[vIndex + 3].Color.set(255, 255, 0, 0);

		vIndex += 4;
	}

	// now render the line list
	this->demo->getVideoDriver()->drawVertexPrimitiveList(vertices, n_vertices, indices, n_indices/2,
		irr::video::EVT_STANDARD, irr::scene::EPT_LINES, irr::video::EIT_16BIT);
}
