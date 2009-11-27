// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef GRASSGENERATOR_H_
#define GRASSGENERATOR_H_

#include "CGrassPatchSceneNode.h"

namespace irr
{

namespace scene
{

class CGrassGeneratorNode : public irr::scene::ISceneNode
{

public:

	/**
	 * Constructor.
	 * \param device A pointer to the Irrlicht device.
	 */
	CGrassGeneratorNode(ISceneManager* const smgr, const bool autoSpawn = false);

	void OnRegisterSceneNode();

	void render();

	const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;

	void addChild(irr::scene::ISceneNode* child);

	void setScale(const core::vector3df& scale);

	void setRotation(const core::vector3df& rotation);
	/**
	 * Adds grass patches to a terrain.
	 * \param wind A pointer to IWindGenerator.
	 * \param terrain A pointer to ITerrainSceneNode.
	 * \param heightMap Use the terrain height map.
	 * \param textureMap .
	 * \param grassMap .
	 */
	const bool addGrassToTerrain(
		irr::scene::IWindGenerator* const wind,
		irr::scene::ITerrainSceneNode* const terrain,
		const irr::video::IImage* const heightMap,
		const irr::video::IImage* const textureMap,
		const irr::video::IImage* const grassMap);

	const void removeAllGrass();

	///Toggle between two textures.
	const void swapTextures();

	/**
	 * Cycle through E_MATERIAL_TYPE modes.
	 * \param forward True if search direction is forward.
	 */
	const void cycleModes(const bool forward = true);


	//setter

	/**
	 * This sets the distance to draw blades at. When a patch is in range,
	 * blades of grass within that square must be checked for distance from
	 * the camera - this is the reason the whole terrain isn't one big
	 * patch.
	 */
	const void decreaseDrawDistance(const irr::f32 dist) const;

	const void increaseDrawDistance(const irr::f32 dist) const;

	/**
	 * This function sets how many wind sensors wide the wind grid is. The
	 * more there are, the nicer the wind effect, but the more it hurts
	 * your cpu. Default is a 5x5 grid. The manager could change this as you
	 * approach and retreat from a patch.
	 */
	const void decreaseWindResolution(const irr::u32 delta = 1) const;

	const void increaseWindResolution(const irr::u32 delta = 1) const;

	/**
	 * This function sets how often the grass is updated. The default is
	 * 30ms, any more looks jerky. If might be an idea to tune this depending on
	 * the distance of the patch to the camera.
	 * If you're using IrrSpintz, hardware mesh buffers (sh|c|w)ould give a
	 * significant performance/drawdistance boost here.
	 */
	const void decreaseRefreshDelay() const;

	const void increaseRefreshDelay() const;

	/**
	 * This sets the maximum number of blades of grass that can be drawn
	 * within the patch. It's probably a good idea to keep this set at the
	 * max - the number made in create(), as its a waste of memory otherwise.
	 * Remember that grass particles are currently uploaded to the card every
	 * frame, the more you send the more it hurts.
	 */
	const void decreaseMaxDensity(const irr::u32 delta = 50) const;

	const void increaseMaxDensity(const irr::u32 delta = 50) const;


	//getter

	//inline irr::core::array<irr::scene::CGrassPatchSceneNode*>& getGrassNodes() { return this->grassNodes; }

	inline const irr::f32 getDrawDistance() const
	{
		return (this->grassNodes.size() > 0) ? this->grassNodes[0]->getDrawDistance() : 0.0f;
	}

	inline const irr::u32 getMaxDensity() const
	{
		return (this->grassNodes.size() > 0) ? this->grassNodes[0]->getMaxDensity() : 0;
	}

	inline const irr::u32 getWindResolution() const
	{
		return (this->grassNodes.size() > 0) ? this->grassNodes[0]->getWindRes() : 0;
	}

	inline const irr::u32 getRefreshDelay() const
	{
		return (this->grassNodes.size() > 0) ? this->grassNodes[0]->getRefreshDelay() : 0;
	}

private:

    ///An array of grass patch nodes.
    core::array<CGrassPatchSceneNode*> grassNodes;

	//material settings
	core::array<video::E_MATERIAL_TYPE> modes;
	u32 currentMode;

	//texture settings
	video::ITexture* tex1;
	video::ITexture* tex2;
    bool tex; //which texture to use;


    core::aabbox3d<f32> Box;

    u32 frameCount;

    ///True if node automatically should spawn grass near the camera
    bool autoSpawn;
};

} // end namespace scene
} // end namespace irr

#endif /*GRASSGENERATOR_H_*/
