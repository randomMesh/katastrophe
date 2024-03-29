// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef OPTIONSSTATE_H_
#define OPTIONSSTATE_H_

#include "../core/statemanager/State.h"

namespace irr
{
	class SEvent;

	namespace gui
	{
		class IGUIWindow;
		class IGUIButton;
		class IGUIComboBox;
		class IGUICheckBox;
		class IGUIScrollBar;
		class IGUIEditBox;
		class IGUITab;
	}

	namespace scene
	{
		class ICameraSceneNode;
		class CGrassGeneratorNode;
	}

	namespace video
	{
		class ITexture;
	}
}

class Demo;

class OptionsState : public State<Demo, irr::SEvent>
{

public:

	OptionsState();

	~OptionsState();

	void onEnter(Demo* const demo);

	void onLeave(Demo* const demo);

	const bool onEvent(Demo* const demo, const irr::SEvent& event);

	void rttCallback(Demo* const demo) const;

private:

	void saveSettings(Demo* const demo) const;


	irr::scene::ICameraSceneNode* camera;

	irr::scene::CGrassGeneratorNode* grassGeneratorNode;


	irr::gui::IGUIWindow* window;

	irr::gui::IGUIButton* backButton;

	irr::gui::IGUIButton* saveButton;



	//flock rules tab

	irr::gui::IGUIEditBox* speedLimitBox;

	irr::gui::IGUIEditBox* scatterFlockBox;

	irr::gui::IGUIEditBox* seekCenterBox;

	irr::gui::IGUIEditBox* distanceBox;

	irr::gui::IGUIEditBox* matchVelocityBox;

	irr::gui::IGUIEditBox* tendencyTowardsBox;

	irr::gui::IGUIEditBox* tendencyAvoidBox;

	irr::gui::IGUIEditBox* aboveGroundBox;

	irr::gui::IGUIButton* resetButton;

	//video tab

	irr::gui::IGUIComboBox* driverSelectionBox;

	irr::gui::IGUIComboBox* resolutionSelectionBox;

	irr::gui::IGUICheckBox* fullscreenBox;

	irr::gui::IGUICheckBox* stencilbufferBox;

	irr::gui::IGUICheckBox* vsyncBox;


	//camera tab

	irr::gui::IGUIEditBox* cameraRotateSpeedBox;

	irr::gui::IGUIEditBox* cameraMoveSpeedBox;

	irr::gui::IGUIEditBox* cameraJumpSpeedBox;

	irr::gui::IGUICheckBox* invertCameraBox;

	//grass tab

	irr::gui::IGUITab* grassTab;

	///Extra camera for the renderTarget.
	irr::scene::ICameraSceneNode* nearCam;

	irr::video::ITexture* renderTarget;

	irr::gui::IGUICheckBox* grassTexture1Box;

	irr::gui::IGUICheckBox* grassTexture2Box;

#ifdef _SOUND
	//sound tab

	irr::gui::IGUICheckBox* soundBox;

	irr::gui::IGUIScrollBar* volumeBar;
#endif
};

#endif /* OPTIONSSTATE_H_ */
