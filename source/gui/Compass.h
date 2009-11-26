// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef COMPASS_H_
#define COMPASS_H_

#include <IGUIElement.h>
#include <SMeshBuffer.h>

namespace irr
{
namespace gui
{

/**
 * Displays a compass which is derived from IGUIElement.
 *
 * Code is originally from vitek and drac_gd
 * See http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=19005
 */
class Compass : public IGUIElement
{

public:

	Compass(const core::rect<s32>& rect, IGUIEnvironment* const env, IGUIElement* const parent);

	//! render the compass
	void draw();

	//! set body of compass texture
	inline void SetCompassBodyTexture(video::ITexture* const texture)
	{
	    BodyMesh.getMaterial().setTexture(0, texture);
	}

	//! set needle of compass texture
	inline void SetCompassNeedleTexture(video::ITexture* const texture)
	{
	    NeedleMesh.getMaterial().setTexture(0, texture);
	}

	//! set compass heading
	inline void SetCompassHeading(const irr::f32 deg)
	{
	    //Matrix.makeIdentity();
	    Matrix.setRotationDegrees(core::vector3df(0.0f, 0.0f, deg));
	}

private:

	//! sets up a quad mesh of given dimension
	void SetupQuadMesh(scene::SMeshBuffer& mesh, const f32 f32Width) const;

	scene::SMeshBuffer BodyMesh;

	scene::SMeshBuffer NeedleMesh;

	core::matrix4 Matrix;
};

}
}

#endif /*COMPASS_H_*/
