// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "Compass.h"
#include <IGUIEnvironment.h>
#include <IVideoDriver.h>

namespace irr
{
namespace gui
{


Compass::Compass(const core::rect<s32>& rect, IGUIEnvironment* const env, IGUIElement* const parent) :
	IGUIElement(EGUIET_ELEMENT, env, parent, -1, rect)
{
	SetupQuadMesh(BodyMesh, 1.0f);
	SetupQuadMesh(NeedleMesh, 1.0f);
}

//! render the compass
void Compass::draw()
{
	if (!IsVisible)
		return;

	video::IVideoDriver* const driver = Environment->getVideoDriver();

	const core::rect<irr::s32> oldViewPort = driver->getViewPort();
	driver->setViewPort(AbsoluteRect);

	// clear the projection matrix
	const core::matrix4& oldProjMat = driver->getTransform(video::ETS_PROJECTION);
	driver->setTransform(video::ETS_PROJECTION, core::matrix4());


	// clear the view matrix
	const core::matrix4& oldViewMat = driver->getTransform(video::ETS_VIEW);
	driver->setTransform(video::ETS_VIEW, core::matrix4());
	driver->setTransform(video::ETS_WORLD, Matrix);

	// draw compass body
	driver->setMaterial(BodyMesh.Material);
	driver->drawMeshBuffer(&BodyMesh);

	driver->setTransform(video::ETS_WORLD, core::matrix4());

	// draw the needle
	driver->setMaterial(NeedleMesh.Material);
	driver->drawMeshBuffer(&NeedleMesh);

	// restore view matrix, projection matrix and viewport
	driver->setTransform(video::ETS_VIEW, oldViewMat);
	driver->setTransform(video::ETS_PROJECTION, oldProjMat);
	driver->setViewPort(oldViewPort);
}

void Compass::SetupQuadMesh(scene::SMeshBuffer& mesh, const f32 f32Width) const
{
	const f32 f32HalfWidth = f32Width/2.0f;
	mesh.Vertices.set_used(4);
	mesh.Indices .set_used(6);

	const video::SColor white(255, 255, 255, 255);

	mesh.Vertices[0] = video::S3DVertex(-f32HalfWidth, -f32HalfWidth, 0.0f, 0.0f, 0.0f, 1.0f, white, 0.0f, 1.0f);
	mesh.Vertices[1] = video::S3DVertex(-f32HalfWidth,  f32HalfWidth, 0.0f, 0.0f, 0.0f, 1.0f, white, 0.0f, 0.0f);
	mesh.Vertices[2] = video::S3DVertex( f32HalfWidth,  f32HalfWidth, 0.0f, 0.0f, 0.0f, 1.0f, white, 1.0f, 0.0f);
	mesh.Vertices[3] = video::S3DVertex( f32HalfWidth, -f32HalfWidth, 0.0f, 0.0f, 0.0f, 1.0f, white, 1.0f, 1.0f);

	mesh.Indices[0] = 0;
	mesh.Indices[1] = 1;
	mesh.Indices[2] = 2;

	mesh.Indices[3] = 2;
	mesh.Indices[4] = 3;
	mesh.Indices[5] = 0;

	mesh.getMaterial().Lighting = false;
	mesh.getMaterial().MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
}

}
}
