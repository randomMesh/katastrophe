// Copyright (C) 2009 randomMesh
// This file is part of the "Katastrophe flocking boids demo".
// For conditions of distribution and use, see copyright notice in main.cpp

#include "Configuration.h"
#include <irrlicht.h>

Configuration::Configuration() :
	//setup name and versioning
	projectName(L"Katastrophe 0.02"),

#ifdef _SOUND
	//sound
	soundEnabled(true),
	volume(1.0f),
#endif

	//gui
	guiTransparency(8),

	//camera
	cameraRotateSpeed(100.0f),
	cameraMoveSpeed(0.5f),
	cameraJumpSpeed(15.0f),
	invertMouse(false),

	//flock
	scatterFlockModifier(2.0f),
	seekCenterOfMass(80.0f),
	distanceToOtherBoids(50.0f),
	matchVelocity(80.0f),
	tendencyTowardsPlace(50.0f),
	tendencyAvoidPlace(100.0f),
	minimumAboveGround(200.0f),
	speedLimit(400.0f),

	//grass
	showGrass(false),
	grassTextureNum(0),
	grassMaterialType(0)
{
	//setup sane default values for irrlicht device in case of invalid / not readable configuration file
	this->params.DriverType = irr::video::EDT_OPENGL;
	this->params.WindowSize.Width = 1024;
	this->params.WindowSize.Height = 768;
	this->params.Bits = 24;
	this->params.Fullscreen = false;
	this->params.Stencilbuffer = false;
	this->params.Vsync = false;

	//this->params.AntiAlias = 16;
}

void Configuration::readFromFile(const irr::c8* const fileName)
{
	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_NULL);

	//create xml reader
	irr::io::IXMLReader* xml = device->getFileSystem()->createXMLReader(fileName);
	if (xml == 0)
	{
		device->getLogger()->log(L"Cannot find configuration file. Falling back to defaults.", irr::ELL_WARNING);
		device->drop();

		return;
	}

	while(xml && xml->read())
	{
		if (irr::core::stringw(L"graphic") == xml->getNodeName())
		{
			const irr::u32 driver = xml->getAttributeValueAsInt(L"driver");
			irr::video::E_DRIVER_TYPE driverType;

			switch (driver)
			{
				case 0: driverType = irr::video::EDT_OPENGL; break;
				case 1: driverType = irr::video::EDT_BURNINGSVIDEO; break;
				default: driverType = irr::video::EDT_SOFTWARE; break;
			}

			this->params.DriverType = driverType;
			this->params.Fullscreen = xml->getAttributeValueAsInt(L"fullscreen");
			this->params.Stencilbuffer = xml->getAttributeValueAsInt(L"stencilbuffer");
			this->params.Vsync = xml->getAttributeValueAsInt(L"vsync");
		}

		else if (irr::core::stringw(L"resolution") == xml->getNodeName())
		{
			this->params.WindowSize.Width = xml->getAttributeValueAsInt(L"xres");
			this->params.WindowSize.Height = xml->getAttributeValueAsInt(L"yres");
			///\todo Check resolution

			this->params.Bits = xml->getAttributeValueAsInt(L"bits");

			//Check color depth.
			if ( (this->params.Bits != 16) || (this->params.Bits != 24))
				this->params.Bits = 24;
		}
#ifdef _SOUND
		else if (irr::core::stringw(L"sound") == xml->getNodeName())
		{
			this->soundEnabled = xml->getAttributeValueAsInt(L"enabled");
			this->volume = xml->getAttributeValueAsInt(L"volume")/100.0f;

			if (this->volume > 1.0f)
				this->volume = 1.0f;
		}
#endif
		else if (irr::core::stringw(L"gui") == xml->getNodeName())
		{
			this->guiTransparency = xml->getAttributeValueAsInt(L"transparency");

			//check for sane values
			if (this->guiTransparency > 255) this->guiTransparency = 255;
			else if (this->guiTransparency < 0) this->guiTransparency = 0;
		}

		else if (irr::core::stringw(L"camera") == xml->getNodeName())
		{
			this->cameraRotateSpeed = xml->getAttributeValueAsFloat(L"rotatespeed");
			this->cameraMoveSpeed = xml->getAttributeValueAsFloat(L"movespeed");
			this->cameraJumpSpeed = xml->getAttributeValueAsFloat(L"jumpspeed");
			this->invertMouse = xml->getAttributeValueAsInt(L"invert");

			//TODO: check for sane values
			if (this->cameraRotateSpeed < 0.01f)
				this->cameraRotateSpeed = 0.01f;
			if (this->cameraMoveSpeed < 0.01f)
				this->cameraMoveSpeed = 0.01f;
			if (this->cameraJumpSpeed < 0.01f)
				this->cameraJumpSpeed = 0.01f;
		}

		else if (irr::core::stringw(L"flock") == xml->getNodeName())
		{
			this->scatterFlockModifier = xml->getAttributeValueAsFloat(L"scatterFlockModifier");
			this->seekCenterOfMass = xml->getAttributeValueAsFloat(L"seekCenterOfMass");
			this->distanceToOtherBoids = xml->getAttributeValueAsFloat(L"distanceToOtherBoids");
			this->matchVelocity = xml->getAttributeValueAsFloat(L"matchVelocity");
			this->tendencyTowardsPlace = xml->getAttributeValueAsFloat(L"tendencyTowardsPlace");
			this->tendencyAvoidPlace = xml->getAttributeValueAsFloat(L"tendencyAvoidPlace");
			this->minimumAboveGround = xml->getAttributeValueAsFloat(L"minimumAboveGround");
			this->speedLimit = xml->getAttributeValueAsFloat(L"speedLimit");

			//todo check for sane values
		}

		else if (irr::core::stringw(L"grass") == xml->getNodeName())
		{
			this->showGrass = xml->getAttributeValueAsInt(L"show");
			this->grassTextureNum = xml->getAttributeValueAsInt(L"texture");
			this->grassMaterialType = xml->getAttributeValueAsInt(L"material");

			if (this->grassTextureNum < 0 || this->grassTextureNum > 1)
				this->grassTextureNum = 0;

			if (this->grassMaterialType < 0 || this->grassMaterialType > 3)
				this->grassMaterialType = 0;
		}
	}

	xml->drop();
	device->drop();
}

void Configuration::writeToFile(irr::IrrlichtDevice* const device, const irr::c8* const fileName) const
{
	irr::io::IXMLWriter* xmlw = 0;
	if (device->getFileSystem()->existFile(fileName))
		xmlw = device->getFileSystem()->createXMLWriter(fileName);
	else
		device->getLogger()->log(L"Configuration file does not exist. Please create an empty file called like this first.", irr::ELL_WARNING);

	if (xmlw == 0)
	{	//could not open file for writing
		return;
	}

	xmlw->writeXMLHeader();
	xmlw->writeLineBreak();

	xmlw->writeElement(L"configuration", false);
	xmlw->writeLineBreak();
	xmlw->writeLineBreak();

	//write grafic driver config
	xmlw->writeComment(L"Irrlicht device settings: driver can be 0 = OPENGL, 1 = Burning's Video or 2 = Software");
	xmlw->writeLineBreak();

	irr::core::stringw driver(L"");
	switch(this->params.DriverType)
	{
		case irr::video::EDT_OPENGL: driver = L"0"; break;
		case irr::video::EDT_BURNINGSVIDEO: driver = L"1"; break;
		default: driver = L"2";
	}

	xmlw->writeElement(L"graphic", true,
		L"driver", driver.c_str(),
		L"fullscreen", irr::core::stringw(this->params.Fullscreen).c_str(),
		L"stencilbuffer", irr::core::stringw(this->params.Stencilbuffer).c_str(),
		L"vsync", irr::core::stringw(this->params.Vsync).c_str()
	);

	xmlw->writeLineBreak();
	xmlw->writeLineBreak();

	//write resolution and color depth
	xmlw->writeComment(L"Resolution and color depth settings");
	xmlw->writeLineBreak();
	xmlw->writeElement(L"resolution", true,
		L"xres", irr::core::stringw(this->params.WindowSize.Width).c_str(),
		L"yres", irr::core::stringw(this->params.WindowSize.Height).c_str(),
		L"bits", irr::core::stringw(this->params.Bits).c_str()
	);

	xmlw->writeLineBreak();
	xmlw->writeLineBreak();

#ifdef _SOUND
	//write sound settings
	xmlw->writeComment(L"Sound settings: enabled=<0|1> volume=<0-100%>");
	xmlw->writeLineBreak();
	xmlw->writeElement(L"sound", true,
		L"enabled", irr::core::stringw(this->soundEnabled).c_str(),
		L"volume", irr::core::stringw((irr::u32)(this->volume*100.0f)).c_str());

	xmlw->writeLineBreak();
	xmlw->writeLineBreak();
#endif

	//write gui settings
	xmlw->writeComment(L"Gui settings: transparency=<0-255>");
	xmlw->writeLineBreak();
	xmlw->writeElement(L"gui", true, L"transparency", irr::core::stringw(this->guiTransparency).c_str());

	xmlw->writeLineBreak();
	xmlw->writeLineBreak();




	//write camera settings
	irr::core::array<irr::core::stringw> cameraNames;
	cameraNames.push_back(L"rotatespeed");
	cameraNames.push_back(L"movespeed");
	cameraNames.push_back(L"jumpspeed");
	cameraNames.push_back(L"invert");
	irr::core::array<irr::core::stringw> cameraValues;
	cameraValues.push_back(irr::core::stringw(this->cameraRotateSpeed));
	cameraValues.push_back(irr::core::stringw(this->cameraMoveSpeed));
	cameraValues.push_back(irr::core::stringw(this->cameraJumpSpeed));
	cameraValues.push_back(irr::core::stringw(this->invertMouse));

	xmlw->writeComment(L"Camera settings: rotatespeed=<todo> movespeed=<0.01-5.0> jumpspeed=<todo> invert=<0|1>");
	xmlw->writeLineBreak();
	xmlw->writeElement(L"camera", true, cameraNames, cameraValues);

	xmlw->writeLineBreak();
	xmlw->writeLineBreak();


	//write flock settings
	xmlw->writeComment(L"Flock settings: todo");
	xmlw->writeLineBreak();
	//xmlw->writeElement(L"flock", true, L"speed", irr::core::stringw(this->cameraMoveSpeed).c_str());

	irr::core::array<irr::core::stringw> names;
	names.push_back(L"scatterFlockModifier");
	names.push_back(L"seekCenterOfMass");
	names.push_back(L"distanceToOtherBoids");
	names.push_back(L"matchVelocity");
	names.push_back(L"tendencyTowardsPlace");
	names.push_back(L"tendencyAvoidPlace");
	names.push_back(L"minimumAboveGround");
	names.push_back(L"speedLimit");

	irr::core::array<irr::core::stringw> values;
	values.push_back(irr::core::stringw(this->scatterFlockModifier));
	values.push_back(irr::core::stringw(this->seekCenterOfMass));
	values.push_back(irr::core::stringw(this->distanceToOtherBoids));
	values.push_back(irr::core::stringw(this->matchVelocity));
	values.push_back(irr::core::stringw(this->tendencyTowardsPlace));
	values.push_back(irr::core::stringw(this->tendencyAvoidPlace));
	values.push_back(irr::core::stringw(this->minimumAboveGround));
	values.push_back(irr::core::stringw(this->speedLimit));

	xmlw->writeElement(L"flock", true, names, values);

	xmlw->writeLineBreak();
	xmlw->writeLineBreak();


	//write grass settings
	xmlw->writeComment(L"Grass settings: todo");
	xmlw->writeLineBreak();
	xmlw->writeElement(L"grass", true,
		L"show", irr::core::stringw(this->showGrass).c_str(),
		L"texture", irr::core::stringw(this->grassTextureNum).c_str(),
		L"material", irr::core::stringw(this->grassMaterialType).c_str());

	xmlw->writeLineBreak();
	xmlw->writeLineBreak();


	xmlw->writeClosingTag(L"configuration");

	xmlw->writeLineBreak();

	//clean up writer
	xmlw->drop();
}
