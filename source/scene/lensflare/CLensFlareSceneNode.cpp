// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

// Original code and artwork by Paulo Oliveira (ProSoft, http://br.geocities.com/paulo_cmv/)
// The code is Public Domain.
// With updates from gammaray and torleif

#include "CLensFlareSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "ISceneCollisionManager.h"
#include "ICameraSceneNode.h"

namespace irr
{
namespace scene
{

/* Creates a Lens Flare node
\param parent, the node to extend (can be null)
\param mgr the scene manager to call functions from
\param id this scenes ID
\param position starting position
*/
CLensFlareSceneNode::CLensFlareSceneNode(
	ISceneNode* parent, ISceneManager* mgr, s32 id, const core::vector3df& position) :
	ILensFlareSceneNode(parent, mgr, id, position), StartPosition(position)
{
	AutomaticCullingState = EAC_BOX;

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 2;

	vertices[0].TCoords.set(0.0f, 1.0f);
	vertices[0].Color = 0xffffffff;
	vertices[1].TCoords.set(0.0f, 0.0f);
	vertices[1].Color = 0xffffffff;
	vertices[2].TCoords.set(1.0f, 0.0f);
	vertices[2].Color = 0xffffffff;
	vertices[3].TCoords.set(1.0f, 1.0f);
	vertices[3].Color = 0xffffffff;

	Screensize = SceneManager->getVideoDriver()->getScreenSize();
	Material.Lighting = false;
	Material.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
	Material.ZBuffer = false;
	Material.ZWriteEnable = false;
	BBox.MinEdge = core::vector3df(-2.0f, -2.0f, -2.0f);
	BBox.MaxEdge = core::vector3df(2.0f, 2.0f, 2.0f);

	this->collmgr = mgr->getSceneCollisionManager();
}

void CLensFlareSceneNode::OnRegisterSceneNode()
{
	if(IsVisible)
	{
		if(core::rect<s32>(core::position2d<s32>(0, 0), Screensize).isPointInside(
			this->collmgr->getScreenCoordinatesFrom3DPosition(
				this->AbsoluteTransformation.getTranslation(), SceneManager->getActiveCamera())))
		{
			SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);
			ISceneNode::OnRegisterSceneNode();
		}
	}
}


/* Every time it gets drawn, this function gets called */
void CLensFlareSceneNode::render()
{
	video::IVideoDriver* const driver = SceneManager->getVideoDriver();
	ICameraSceneNode* const camera = SceneManager->getActiveCamera();

	const core::vector3df& campos = camera->getAbsolutePosition();
	const core::dimension2du& sz = driver->getScreenSize();
	const core::vector2df& mid = core::vector2df((f32)sz.Width, (f32)sz.Height)*0.5f;

	const core::position2di& lp = this->collmgr->getScreenCoordinatesFrom3DPosition(getAbsolutePosition(), camera);
	const core::vector2df& lightpos = core::vector2df((f32)lp.X, (f32)lp.Y);


	const u32 nframes = Material.getTexture(0)->getOriginalSize().Width/Material.getTexture(0)->getOriginalSize().Height;
	int texw = 8;
	const irr::f32 texp = 1.0f/nframes;

	const core::vector3df& target = camera->getTarget();
	const core::vector3df& up = camera->getUpVector();
	core::vector3df view = target - campos;
	view.normalize();

	core::vector3df horizontal = up.crossProduct(view);
	horizontal.normalize();

	core::vector3df vertical;
	vertical = horizontal.crossProduct(view);
	vertical.normalize();
	view *= -1.0f;

	for (u32 i = 0; i < 4; ++i)
		vertices[i].Normal = view;

	driver->setTransform(video::ETS_WORLD, core::matrix4());
	driver->setMaterial(Material);
	core::vector3df pos;

	for (u32 ax = 0; ax < nframes; ++ax)
	{
		if (ax == 0 )
		{
			pos = this->AbsoluteTransformation.getTranslation();
			texw = Material.getTexture(0)->getSize().Height;
		}
		else
		{
			const core::vector2df& ipos = mid.getInterpolated(lightpos, (2.0f/nframes)*ax);

			pos = this->collmgr->getRayFromScreenCoordinates(core::position2d<s32>(int(ipos.X ), int(ipos.Y ) ), camera).end;

			core::vector3df dir = core::vector3df(campos - pos).normalize();
			pos = campos + (dir * -10.0f);
			texw = 4;
		}

		vertices[0].TCoords.set(ax*texp, 1.0f);
		vertices[1].TCoords.set(ax*texp, 0.0f);
		vertices[2].TCoords.set((ax+1)*texp, 0.0f);
		vertices[3].TCoords.set((ax+1)*texp, 1.0f);

		const core::vector3df hor = horizontal*(0.5f*texw);
		const core::vector3df ver = vertical*(0.5f*texw);

		vertices[0].Pos = pos + hor + ver;
		vertices[1].Pos = pos + hor - ver;
		vertices[2].Pos = pos - hor - ver;
		vertices[3].Pos = pos - hor + ver;

		driver->drawIndexedTriangleList(vertices, 4, indices, 2);
	}
}


// returns a 'bounding box' of this node
const core::aabbox3df& CLensFlareSceneNode::getBoundingBox() const
{
	return BBox;
}


// resets the screen size
void CLensFlareSceneNode::setScreenSize(const core::dimension2d<s32>& newSize)
{
	Screensize = newSize;
}


}// end irr namespace
}// end scene namespace
