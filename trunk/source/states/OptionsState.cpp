// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "OptionsState.h"
#include <IrrlichtDevice.h>

#include <ISceneManager.h>
#include <ITerrainSceneNode.h>
#include <ICameraSceneNode.h>
#include <IMeshCache.h>
#include <IVideoDriver.h>
#include <IGUIEnvironment.h>
#include <IGUIWindow.h>
#include <IGUIButton.h>
#include <IGUICheckBox.h>
#include <IGUIEditBox.h>
#include <IGUIStaticText.h>
#include <IGUIComboBox.h>
#include <IGUIImage.h>
#include <IGUIScrollBar.h>
#include <IGUITabControl.h>
#include "../core/Demo.h"
#include "../core/Configuration.h"

#include "../scene/grass/CGrassGeneratorNode.h"
#include "../scene/grass/CWindGenerator.h"

#ifdef _SOUND
#include <ik_ISoundEngine.h>
#endif

OptionsState::OptionsState() :

camera(0), grassGeneratorNode(0),

window(0), backButton(0), saveButton(0),

//flock rules tab
speedLimitBox(0), scatterFlockBox(0), seekCenterBox(0), distanceBox(0),
matchVelocityBox(0), tendencyTowardsBox(0), tendencyAvoidBox(0), aboveGroundBox(0),

//video tab
driverSelectionBox(0), resolutionSelectionBox(0), fullscreenBox(0), stencilbufferBox(0), vsyncBox(0),

//camera tab
cameraRotateSpeedBox(0), cameraMoveSpeedBox(0), cameraJumpSpeedBox(0), invertCameraBox(0),

//grass tab
grassTab(0), nearCam(0), renderTarget(0), grassTexture1Box(0), grassTexture2Box(0)

#ifdef _SOUND
//sound tab
, soundBox(0), volumeBar(0)
#endif
{

}

OptionsState::~OptionsState()
{
	if (this->window)
	{
		this->window->remove();
		this->window->drop();
	}

	if (this->backButton)
	{
		this->backButton->remove();
		this->backButton->drop();
	}

	if (this->saveButton)
	{
		this->saveButton->remove();
		this->saveButton->drop();
	}

	if (grassGeneratorNode)
	{
		grassGeneratorNode->remove();
		grassGeneratorNode->drop();
	}
}

void OptionsState::onEnter(Demo* const demo)
{
	irr::gui::IGUIEnvironment* const guienv = demo->getGuiEnvironment();
	irr::video::IVideoDriver* const driver = demo->getVideoDriver();
	irr::scene::ISceneManager* const smgr = demo->getSceneManager();

	const Configuration* const config = demo->getConfiguration();



	smgr->setAmbientLight(irr::video::SColorf(0.9f, 0.9f, 0.9f));

	this->camera = smgr->addCameraSceneNode(0, irr::core::vector3df(1000.0f, 200.0f, 1000.0f), irr::core::vector3df(-500.0f, 100, 500.0f));

	//create an animated skybox
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	irr::scene::ISceneNode* const skybox = smgr->addSkyBoxSceneNode(
			driver->getTexture("media/images/skybox/starfield/starfield_up.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_dn.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_lf.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_rt.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_ft.png"),
			driver->getTexture("media/images/skybox/starfield/starfield_bk.png"));
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);

	irr::scene::ISceneNodeAnimator* const skyAnim = smgr->createRotationAnimator(irr::core::vector3df(0.0f, 0.01f, 0.0f));
	skybox->addAnimator(skyAnim);
	skyAnim->drop();

	//add terrain scene node
	irr::scene::ITerrainSceneNode* const terrain = smgr->addTerrainSceneNode(
			"media/images/terrain/terrain-heightmap.bmp",
			0,											// parent node
			-1,											// node id
			irr::core::vector3df(0.0f, 0.0f, 0.0f),		// position
			irr::core::vector3df(0.0f, 0.0f, 0.0f),		// rotation
			irr::core::vector3df(40.0f, 2.0f, 40.0f),	// scale
			irr::video::SColor(255, 255, 255, 255),		// vertexColor
			5,											// maxLOD
			irr::scene::ETPS_17,						// patchSize
			4);											// smoothFactor
	terrain->setMaterialTexture(0, driver->getTexture("media/images/terrain/terrain-texture.jpg"));
	terrain->setMaterialTexture(1, driver->getTexture("media/images/terrain/detailmap3.jpg"));
	terrain->setMaterialType(irr::video::EMT_DETAIL_MAP);
	terrain->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	terrain->scaleTexture(1.0f, 50.0f);


	// create wind
	irr::scene::CWindGenerator* const wind = new irr::scene::CWindGenerator;
	wind->setStrength(30.0f);
	wind->setRegularity(3.0f);


	//add grass
	const irr::video::IImage* const heightMap = driver->createImageFromFile("media/images/terrain/terrain-heightmap.bmp");
	const irr::video::IImage* const textureMap = driver->createImageFromFile("media/images/terrain/terrain-grasscol.bmp");
	const irr::video::IImage* const grassMap = driver->createImageFromFile("media/images/terrain/terrain-grassmap.png");

	this->grassGeneratorNode = new irr::scene::CGrassGeneratorNode(smgr);
	this->grassGeneratorNode->addGrassToTerrain(wind, terrain, heightMap, textureMap, grassMap);


	heightMap->drop();
	textureMap->drop();
	grassMap->drop();

	wind->drop();








	//add back button
	irr::video::ITexture* const buttonTexture = driver->getTexture("media/images/button.png");
	if (buttonTexture)
	{
		const irr::core::dimension2du& buttonSize = buttonTexture->getSize();

		this->backButton = guienv->addButton(irr::core::rect<irr::s32>(SX(20), SY(230), SX(20) + buttonSize.Width, SY(230) + buttonSize.Height), 0, -1, L"Back [ESC]");
		this->backButton->setImage(buttonTexture);
		this->backButton->setUseAlphaChannel(true);
		this->backButton->setDrawBorder(false);
		this->backButton->grab();


		this->saveButton = guienv->addButton(irr::core::rect<irr::s32>(SX(20), SY(280), SX(20) + buttonSize.Width, SY(280) + buttonSize.Height), 0, -1, L"[S]ave");
		this->saveButton->setImage(buttonTexture);
		this->saveButton->setUseAlphaChannel(true);
		this->saveButton->setDrawBorder(false);
		this->saveButton->grab();
	}


	//add centered menu window
	const irr::core::dimension2du& screenSize = driver->getScreenSize();
	const irr::core::dimension2du windowSize(SX(600), SY(500));
	const irr::u32 winPosX = (screenSize.Width - windowSize.Width)/2;
	const irr::u32 winPosY = (screenSize.Height - windowSize.Height)/2;
	const irr::core::rect<irr::s32> windowRect(winPosX, winPosY, winPosX + windowSize.Width, winPosY + windowSize.Height);

	this->window = guienv->addWindow(windowRect, false, L"Options - About", 0, -1);
	this->window->getCloseButton()->remove();
	this->window->setDraggable(false);
	this->window->grab();

	//add tabcontrol to window
	const irr::core::rect<irr::s32> tabControlSize(20, 20, windowSize.Width - 20, windowSize.Height - 20);
	irr::gui::IGUITabControl* const tabControl = guienv->addTabControl(tabControlSize, this->window, true, true, -1);


	const irr::u32 tabControlWidth = tabControl->getAbsolutePosition().getWidth();

	static const irr::video::SColor titleColor(255, 0, 0, 255);

	//about tab
	{
		irr::gui::IGUITab* const infoTab = tabControl->addTab(L"About", -1);

		const irr::core::rect<irr::s32> textSize(
				20, 20, infoTab->getAbsolutePosition().getWidth() - 20, infoTab->getAbsolutePosition().getHeight() - 20);

		static const irr::core::stringw about(
				L"Boids, developed by Craig Reynolds (http://www.red3d.com/cwr/) in 1986, is an artificial life program, simulating the flocking behaviour of birds.\n\n"
				"Katastrophe: Flocking boids is an implementation of the pseudocode found at the website of Conrad Parker.\n"
				"See http://www.vergenet.net/~conrad/boids/pseudocode.html\n\n"
				"There are 3 rules which a boid in a flock has to follow:\n"
				"    Separation: steer to avoid crowding local flockmates\n"
				"    Alignment: steer towards the average heading of local flockmates\n"
				"    Cohesion:   steer to move toward the average position of local flockmates");

		guienv->addStaticText(about.c_str(), textSize, true, true, infoTab, -1, true);
	}


	const irr::u32 textWidth = 160;
	const irr::u32 spacer = 20;
	const irr::u32 horizontalBorder = 40;
	const irr::u32 posX = horizontalBorder + textWidth;
	const irr::u32 posX2 = tabControlWidth - horizontalBorder;
	const irr::u32 depp = posX + spacer;

	irr::gui::IGUIStaticText* text = 0;


	//flock rules tab
	{
		irr::gui::IGUITab* const rulesTab = tabControl->addTab(L"Flock rules", -1);


		//title text
		text = guienv->addStaticText(
				L"You are going to edit the rules of the flock. Be careful!",
				irr::core::rect<irr::s32>(horizontalBorder, 20, posX2, 40), false, false, rulesTab);
		text->setOverrideColor(irr::video::SColor(255, 255, 0, 0));


		text = guienv->addStaticText(L"Speed limit",
				irr::core::rect<irr::s32>(horizontalBorder, 50, posX, 70), false, false, rulesTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
		text->setToolTipText(L"Set the speed limit of the boids");

		text = guienv->addStaticText(L"Scatter flock modifier",
				irr::core::rect<irr::s32>(horizontalBorder, 80, posX, 100), false, false, rulesTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
		text->setToolTipText(L"Set the scatter flock modifier");

		text = guienv->addStaticText(L"Seek center of mass",
				irr::core::rect<irr::s32>(horizontalBorder, 110, posX, 130), false, false, rulesTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
		text->setToolTipText(L"Set the seek center of mass modifier");

		text = guienv->addStaticText(L"Distance to other",
				irr::core::rect<irr::s32>(horizontalBorder, 140, posX, 160), false, false, rulesTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
		text->setToolTipText(L"Distance to other boids");

		text = guienv->addStaticText(L"Match velocity",
				irr::core::rect<irr::s32>(horizontalBorder, 170, posX, 190), false, false, rulesTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
		text->setToolTipText(L"Match velocity");

		text = guienv->addStaticText(L"Tendency towards place",
				irr::core::rect<irr::s32>(horizontalBorder, 200, posX, 220), false, false, rulesTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
		text->setToolTipText(L"Tendency towards place");

		text = guienv->addStaticText(L"Tendency avoid place",
				irr::core::rect<irr::s32>(horizontalBorder, 230, posX, 250), false, false, rulesTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
		text->setToolTipText(L"Tendency avoid place");

		text = guienv->addStaticText(L"Minimum above ground",
				irr::core::rect<irr::s32>(horizontalBorder, 260, posX, 280), false, false, rulesTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
		text->setToolTipText(L"Minimum height above ground");



		this->speedLimitBox = guienv->addEditBox(
				irr::core::stringw(config->getSpeedLimit()).c_str(),
				irr::core::rect<irr::s32>(depp, 50, posX2, 70), true, rulesTab);

		this->scatterFlockBox = guienv->addEditBox(
				irr::core::stringw(config->getScatterFlockModifier()).c_str(),
				irr::core::rect<irr::s32>(depp, 80, posX2, 100), true, rulesTab);

		this->seekCenterBox = guienv->addEditBox(
				irr::core::stringw(config->getSeekCenterOfMass()).c_str(),
				irr::core::rect<irr::s32>(depp, 110, posX2, 130), true, rulesTab);

		this->distanceBox = guienv->addEditBox(
				irr::core::stringw(config->getDistanceToOtherBoids()).c_str(),
				irr::core::rect<irr::s32>(depp, 140, posX2, 160), true, rulesTab);

		this->matchVelocityBox = guienv->addEditBox(
				irr::core::stringw(config->getMatchVelocity()).c_str(),
				irr::core::rect<irr::s32>(depp, 170, posX2, 190), true, rulesTab);

		this->tendencyTowardsBox = guienv->addEditBox(
				irr::core::stringw(config->getTendencyTowardsPlace()).c_str(),
				irr::core::rect<irr::s32>(depp, 200, posX2, 220), true, rulesTab);

		this->tendencyAvoidBox = guienv->addEditBox(
				irr::core::stringw(config->getTendencyAvoidPlace()).c_str(),
				irr::core::rect<irr::s32>(depp, 230, posX2, 250), true, rulesTab);

		this->aboveGroundBox = guienv->addEditBox(
				irr::core::stringw(config->getMimimumAboveGround()).c_str(),
				irr::core::rect<irr::s32>(depp, 260, posX2, 280), true, rulesTab);
	}

	//video tab
	{
		irr::gui::IGUITab* const videoTab = tabControl->addTab(L"Video", -1);



		//title text
		text = guienv->addStaticText(
				L"Edit video settings. You need to restart for changes to take effect.",
				irr::core::rect<irr::s32>(horizontalBorder, 20, posX2, 40), false, false, videoTab);
		text->setOverrideColor(titleColor);


		text = guienv->addStaticText(L"Graphic driver",
				irr::core::rect<irr::s32>(horizontalBorder, 50, posX, 70), false, false, videoTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

		text = guienv->addStaticText(L"Screen resolution",
				irr::core::rect<irr::s32>(horizontalBorder, 80, posX, 100), false, false, videoTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

		text = guienv->addStaticText(L"Fullscreen",
				irr::core::rect<irr::s32>(horizontalBorder, 110, posX, 130), false, false, videoTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

		text = guienv->addStaticText(L"Realtime shadows",
				irr::core::rect<irr::s32>(horizontalBorder, 140, posX, 160), false, false, videoTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

		text = guienv->addStaticText(L"Vertical sync",
				irr::core::rect<irr::s32>(horizontalBorder, 170, posX, 190), false, false, videoTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);


		this->driverSelectionBox = guienv->addComboBox(irr::core::rect<irr::s32>(depp, 50, posX2, 70), videoTab);
		this->driverSelectionBox->addItem(L"OpenGL");
		this->driverSelectionBox->addItem(L"Burnings Video");
		this->driverSelectionBox->addItem(L"Software renderer");


		const irr::SIrrlichtCreationParameters& irrlichtParams = config->getIrrlichtParams();
		switch(irrlichtParams.DriverType)
		{
		case irr::video::EDT_OPENGL: this->driverSelectionBox->setSelected(0); break;
		case irr::video::EDT_BURNINGSVIDEO: this->driverSelectionBox->setSelected(1); break;
		default: this->driverSelectionBox->setSelected(2);
		}

		this->resolutionSelectionBox = guienv->addComboBox(
				irr::core::rect<irr::s32>(depp, 80, posX2, 100), videoTab);


		//find resolution in config file
		const irr::core::dimension2du& configResolution = irrlichtParams.WindowSize;
		irr::s32 index = 0;

		irr::video::IVideoModeList* const videoModeList = demo->getDevice()->getVideoModeList();
		const irr::s32 videoModeCount = videoModeList->getVideoModeCount();

		irr::s32 i;
		for (i = 0; i < videoModeCount; ++i)
		{
			const irr::core::dimension2du& videoModeResolution = videoModeList->getVideoModeResolution(i);
			const irr::s32 videoModeDepth = videoModeList->getVideoModeDepth(i);

			irr::core::stringw res(videoModeResolution.Width);
			res += L"x";
			res += videoModeResolution.Height;
			res += L"x";
			res += videoModeDepth;

			this->resolutionSelectionBox->addItem(res.c_str());

			if (videoModeResolution == configResolution)
				index = i;
		}

		this->resolutionSelectionBox->setSelected(index);


		//fullscreen checkbox
		this->fullscreenBox = guienv->addCheckBox(irrlichtParams.Fullscreen,
				irr::core::rect<irr::s32>(depp, 110, depp + 20, 130), videoTab, -1, L"");

		//realtime shadows
		this->stencilbufferBox = guienv->addCheckBox(irrlichtParams.Stencilbuffer,
				irr::core::rect<irr::s32>(depp, 140, depp + 20, 160), videoTab, -1, L"");

		//vertical sync
		this->vsyncBox = guienv->addCheckBox(irrlichtParams.Vsync,
				irr::core::rect<irr::s32>(depp, 170, depp + 20, 190), videoTab, -1, L"");


		// add irrlicht logo
		irr::video::ITexture* const logo = driver->getTexture("media/images/logo/irrlichtlogo3.png");
		if (logo)
		{
			const irr::core::dimension2d<irr::u32>& imageSize = logo->getSize();
			const irr::u32 posX = (tabControlWidth - imageSize.Width)/2;
			guienv->addImage(logo, irr::core::position2d<irr::s32>(posX, 210), true, videoTab);
		}
	}

	//camera tab
	{
		irr::gui::IGUITab* const cameraTab = tabControl->addTab(L"Camera", -1);


		//title text
		text = guienv->addStaticText(
				L"Edit camera settings.",
				irr::core::rect<irr::s32>(horizontalBorder, 20, posX2, 40), false, false, cameraTab);
		text->setOverrideColor(titleColor);


		text = guienv->addStaticText(L"Rotate speed",
				irr::core::rect<irr::s32>(horizontalBorder, 50, posX, 70), false, false, cameraTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

		text = guienv->addStaticText(L"Move speed",
				irr::core::rect<irr::s32>(horizontalBorder, 80, posX, 100), false, false, cameraTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

		text = guienv->addStaticText(L"Jump speed",
				irr::core::rect<irr::s32>(horizontalBorder, 110, posX, 130), false, false, cameraTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

		text = guienv->addStaticText(L"Invert mouse",
				irr::core::rect<irr::s32>(horizontalBorder, 140, posX, 160), false, false, cameraTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);


		this->cameraRotateSpeedBox = guienv->addEditBox(
				irr::core::stringw(config->getCameraRotateSpeed()).c_str(),
				irr::core::rect<irr::s32>(depp, 50, posX2, 70), true, cameraTab);

		this->cameraMoveSpeedBox = guienv->addEditBox(
				irr::core::stringw(config->getCameraMoveSpeed()).c_str(),
				irr::core::rect<irr::s32>(depp, 80, posX2, 100), true, cameraTab);

		this->cameraJumpSpeedBox = guienv->addEditBox(
				irr::core::stringw(config->getCameraJumpSpeed()).c_str(),
				irr::core::rect<irr::s32>(depp, 110, posX2, 130), true, cameraTab);

		this->invertCameraBox = guienv->addCheckBox(config->isInvertMouse(),
				irr::core::rect<irr::s32>(depp, 140, depp + 20, 160), cameraTab);
	}


	//grass tab
	{
		this->grassTab = tabControl->addTab(L"Grass", -1);


		//title text
		text = guienv->addStaticText(L"Edit grass settings.",
				irr::core::rect<irr::s32>(horizontalBorder, 20, posX2, 40), false, false, grassTab);
		text->setOverrideColor(titleColor);


		if (driver->queryFeature(irr::video::EVDF_RENDER_TO_TARGET))
		{
			//add preview
			this->renderTarget = driver->addRenderTargetTexture(irr::core::dimension2d<irr::u32>(256, 256), "RTT1");

			guienv->addImage(this->renderTarget, irr::core::vector2d<irr::s32>(horizontalBorder, 50), false, grassTab);

			//add near camera
			this->nearCam = smgr->addCameraSceneNode(0, irr::core::vector3df(100.0f, 150.0f, 100.0f), irr::core::vector3df(1000.0f, 100, 1000.0f));
			//this->nearCam->setFarValue(100.0f);

			//we need to make normal camera active again
			smgr->setActiveCamera(this->camera);
		}
		else
		{
			text = guienv->addStaticText(L"Your hardware or this renderer is not able to use the render to texture feature.\n\nPreview disabled.",
				irr::core::rect<irr::s32>(horizontalBorder, 50, horizontalBorder + 256, 50 + 256), true, true, grassTab);
			text->setOverrideColor(irr::video::SColor(255, 255, 0, 0));
		}


		//texture text
		const irr::u32 meep = horizontalBorder + 256 + spacer;
		text = guienv->addStaticText(L"Texture", irr::core::rect<irr::s32>(meep, 50, posX2, 70), true, false, grassTab);


		//add textures as images
		const irr::core::dimension2du fixedImageSize(64, 64);

		irr::video::ITexture* const texture1 = driver->getTexture("media/images/grass/grass.png");
		if (texture1)
		{
			irr::gui::IGUIImage* const image = guienv->addImage(irr::core::rect<irr::s32>(meep, 80, meep + fixedImageSize.Width, 80 + fixedImageSize.Height), grassTab);
			image->setScaleImage(true);
			image->setUseAlphaChannel(true);
			image->setImage(texture1);

			this->grassTexture1Box = guienv->addCheckBox(true,
				irr::core::rect<irr::s32>(meep + fixedImageSize.Width + 10, 80, meep + fixedImageSize.Width + 10 + 20, 100), grassTab);

		}



		irr::video::ITexture* const texture2 = driver->getTexture("media/images/grass/grass2.bmp");
		if (texture2)
		{
			driver->makeColorKeyTexture(texture2, irr::core::position2d<irr::s32>(0, 0));

			irr::gui::IGUIImage* const image = guienv->addImage(irr::core::rect<irr::s32>(
				meep + fixedImageSize.Width + 20 + 20, 80,
				meep + fixedImageSize.Width + 20 + 20 + fixedImageSize.Width, 80 + 64), grassTab);
			image->setScaleImage(true);
			image->setUseAlphaChannel(true);
			image->setImage(texture2);

			this->grassTexture2Box = guienv->addCheckBox(false,
				irr::core::rect<irr::s32>(
					meep + fixedImageSize.Width + 20 + 20 + fixedImageSize.Width + 10, 80,
					meep + fixedImageSize.Width + 20 + 20 + fixedImageSize.Width + 10 + 20, 100), grassTab);
		}



		//material type text
		text = guienv->addStaticText(L"Material type", irr::core::rect<irr::s32>(meep, 170, posX2, 190), true, false, grassTab);

		irr::gui::IGUIComboBox* combo = guienv->addComboBox(irr::core::rect<irr::s32>(meep, 200, posX2, 220), grassTab);
		combo->addItem(L"ALPHA_CHANNEL");
		combo->addItem(L"ALPHA_CHANNEL_REF");
		combo->addItem(L"ADD_COLOR");
		combo->addItem(L"VERTEX_ALPHA");
	}


#ifdef _SOUND
	//sound tab
	{
		irr::gui::IGUITab* const soundTab = tabControl->addTab(L"Sound", -1);



		//title text
		text = guienv->addStaticText(L"Edit audio settings.",
				irr::core::rect<irr::s32>(horizontalBorder, 20, posX2, 40), false, false, soundTab);
		text->setOverrideColor(titleColor);

		text = guienv->addStaticText(L"Enable sound",
				irr::core::rect<irr::s32>(horizontalBorder, 50, posX, 70), false, false, soundTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

		text = guienv->addStaticText(L"Volume",
				irr::core::rect<irr::s32>(horizontalBorder, 80, posX, 100), false, false, soundTab);
		text->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);



		this->soundBox = guienv->addCheckBox(config->isSoundEnabled(),
				irr::core::rect<irr::s32>(depp, 50, depp + 20, 70), soundTab, -1);

		this->volumeBar = guienv->addScrollBar(
				true, irr::core::rect<irr::s32>(depp, 80, posX2, 100), soundTab);
		this->volumeBar->setMax(100);
		this->volumeBar->setMin(0);
		this->volumeBar->setSmallStep(1);
		this->volumeBar->setLargeStep(10);
		this->volumeBar->setPos((irr::s32)(demo->getSoundEngine()->getSoundVolume()*100.0));

		// add irrklang logo
		irr::video::ITexture* const logo = driver->getTexture("media/images/logo/irrklang_small.png");
		if (logo)
		{
			const irr::core::dimension2d<irr::u32>& imageSize = logo->getSize();
			const irr::u32 posX = (tabControlWidth - imageSize.Width)/2;
			guienv->addImage(logo, irr::core::position2d<irr::s32>(posX, 210), true, soundTab);
		}
	}
#endif


	//enable trilinear and anisotrophic filtering
	demo->setTextureFiltering(false, true, true);


	guienv->setFocus(tabControl);
}

void OptionsState::onLeave(Demo* const demo)
{
	irr::scene::ISceneManager* const smgr = demo->getSceneManager();

	//clear scenemanager
	irr::scene::ICameraSceneNode* camera = smgr->getActiveCamera();
	smgr->setActiveCamera(0);
	camera->remove();
	if (camera == this->camera)
		this->camera = 0;
	else
		this->nearCam = 0;
	camera = 0;


	if (this->camera)
	{
		this->camera->remove();
		this->camera = 0;
	}

	if (this->renderTarget)
	{
		if (this->nearCam)
		{
			this->nearCam->remove();
			this->nearCam = 0;
		}

		demo->getVideoDriver()->removeTexture(this->renderTarget);
		this->renderTarget = 0;
	}

	this->grassGeneratorNode->remove();
	this->grassGeneratorNode->drop();
	this->grassGeneratorNode = 0;

	smgr->getMeshCache()->clear();
	smgr->clear();


	this->window->remove();
	this->window->drop();
	this->window = 0;

	this->backButton->remove();
	this->backButton->drop();
	this->backButton = 0;

	this->saveButton->remove();
	this->saveButton->drop();
	this->saveButton = 0;
}

const bool OptionsState::onEvent(Demo* const demo, const irr::SEvent& event)
{
	switch(event.EventType)
	{

	case irr::EET_KEY_INPUT_EVENT:
	{
		if (!event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{

			case irr::KEY_ESCAPE:
			{
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
				demo->setState(demo->findGameState("menu"));
			}
			return true;

			default: return false;
			}
		}
	}
	break;


	case irr::EET_GUI_EVENT:
	{
		if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == this->backButton)
			{
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
				demo->setState(demo->findGameState("menu"));
				return true;
			}

			else if (event.GUIEvent.Caller == this->saveButton)
			{
#ifdef _SOUND
				if (demo->getSoundEngine() && demo->getConfiguration()->isSoundEnabled())
					demo->getSoundEngine()->play2D("media/sounds/button.wav");
#endif
				this->saveSettings(demo);
				return true;
			}
		}


		else if (event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED)
		{
			const irr::gui::EGUI_ELEMENT_TYPE& type = event.GUIEvent.Caller->getType();

			if  (type == irr::gui::EGUIET_BUTTON)
			{
				((irr::gui::IGUIButton*)event.GUIEvent.Caller)->setPressed(true);
				return true;
			}

			else if (type == irr::gui::EGUIET_EDIT_BOX)
			{
				((irr::gui::IGUIEditBox*)event.GUIEvent.Caller)->setOverrideColor(
						demo->getGuiEnvironment()->getSkin()->getColor(irr::gui::EGDC_HIGH_LIGHT));
				return true;
			}
		}

		else if (event.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT)
		{
			const irr::gui::EGUI_ELEMENT_TYPE& type = event.GUIEvent.Caller->getType();

			if  (type == irr::gui::EGUIET_BUTTON)
			{
				((irr::gui::IGUIButton*)event.GUIEvent.Caller)->setPressed(false);
				return true;
			}

			else if (type == irr::gui::EGUIET_EDIT_BOX)
			{
				((irr::gui::IGUIEditBox*)event.GUIEvent.Caller)->enableOverrideColor(false);
				return true;
			}
		}


		else if(event.GUIEvent.EventType == irr::gui::EGET_TAB_CHANGED)
		{
			irr::gui::IGUITabControl* const tabControl = (irr::gui::IGUITabControl* const)event.GUIEvent.Caller;

			irr::core::stringw text(L"Options - ");
			text.append(tabControl->getTab(tabControl->getActiveTab())->getText());

			this->window->setText(text.c_str());
		}


		else if(event.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED)
		{
			irr::gui::IGUIScrollBar* const bar = (irr::gui::IGUIScrollBar* const)event.GUIEvent.Caller;

#ifdef _SOUND
			if (bar == this->volumeBar)
			{
				if (demo->getSoundEngine())
					demo->getSoundEngine()->setSoundVolume(bar->getPos()/100.0f);
			}
#endif
		}


		else if(event.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED)
		{
			irr::gui::IGUICheckBox* const box = (irr::gui::IGUICheckBox* const)event.GUIEvent.Caller;


			if (box == this->grassTexture1Box)
			{
				this->grassGeneratorNode->swapTextures();
				this->grassTexture2Box->setChecked(!this->grassTexture2Box->isChecked());
			}
			else if (box == this->grassTexture2Box)
			{
				this->grassGeneratorNode->swapTextures();
				this->grassTexture1Box->setChecked(!this->grassTexture1Box->isChecked());
			}

#ifdef _SOUND
			if (box == this->soundBox)
			{
				if (this->soundBox->isChecked())
				{
					demo->getConfiguration()->setSoundEnabled(true);
				}
				else
				{
					demo->getSoundEngine()->stopAllSounds();
					demo->getConfiguration()->setSoundEnabled(false);
				}
			}
#endif
		}
	}
	break;

	default: return false;
	}

	return false;
}


void OptionsState::saveSettings(Demo* const demo) const
{
	irr::SIrrlichtCreationParameters params;
	switch(this->driverSelectionBox->getSelected())
	{
	case 0: params.DriverType = irr::video::EDT_OPENGL; break;
	case 1: params.DriverType = irr::video::EDT_BURNINGSVIDEO; break;
	default: params.DriverType = irr::video::EDT_SOFTWARE;
	}

	irr::video::IVideoModeList* const videoModeList = demo->getDevice()->getVideoModeList();
	const irr::s32 selected = this->resolutionSelectionBox->getSelected();

	params.WindowSize = videoModeList->getVideoModeResolution(selected);
	params.Bits = videoModeList->getVideoModeDepth(selected);
	params.Fullscreen = this->fullscreenBox->isChecked();
	params.Stencilbuffer = this->stencilbufferBox->isChecked();
	params.Vsync = this->vsyncBox->isChecked();

	Configuration* const config = demo->getConfiguration();

	config->setIrrlichtParams(params);

#ifdef _SOUND
	config->setSoundEnabled(this->soundBox->isChecked());
	config->setSoundVolume(this->volumeBar->getPos()/100.0f);
#endif

	config->setSpeedLimit(atof(irr::core::stringc(this->speedLimitBox->getText()).c_str()));
	config->setScatterFlockModifier(atof(irr::core::stringc(this->scatterFlockBox->getText()).c_str()));
	config->setSeekCenterOfMass(atof(irr::core::stringc(this->seekCenterBox->getText()).c_str()));
	config->setDistanceToOtherBoids(atof(irr::core::stringc(this->distanceBox->getText()).c_str()));
	config->setMatchVelocity(atof(irr::core::stringc(this->matchVelocityBox->getText()).c_str()));
	config->setTendencyTowardsPlace(atof(irr::core::stringc(this->tendencyTowardsBox->getText()).c_str()));
	config->setTendencyAvoidPlace(atof(irr::core::stringc(this->tendencyAvoidBox->getText()).c_str()));
	config->setMinimumAboveGround(atof(irr::core::stringc(this->aboveGroundBox->getText()).c_str()));

	config->setCameraRotateSpeed(atof(irr::core::stringc(this->cameraRotateSpeedBox->getText()).c_str()));
	config->setCameraMoveSpeed(atof(irr::core::stringc(this->cameraMoveSpeedBox->getText()).c_str()));
	config->setCameraJumpSpeed(atof(irr::core::stringc(this->cameraJumpSpeedBox->getText()).c_str()));
	config->setInvertMouse(this->invertCameraBox->isChecked());

	config->writeToFile(demo->getDevice(), "config.xml");
}

void OptionsState::rttCallback(Demo* const demo) const
{
	//render to rtt
	if (this->renderTarget && this->grassTab->isVisible())
	{
		irr::scene::ISceneManager* const smgr = demo->getSceneManager();
		irr::video::IVideoDriver* const driver = demo->getVideoDriver();

		// set render target texture
		driver->setRenderTarget(this->renderTarget, true, true, irr::video::SColor(0, 0, 0, 255));

		smgr->setActiveCamera(this->nearCam);

		// draw scene into render buffer
		smgr->drawAll();

		// set back old render target. The buffer might have been distorted, so clear it
		driver->setRenderTarget(0, true, true, 0);

		smgr->setActiveCamera(this->camera);
	}
}
