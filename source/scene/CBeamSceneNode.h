// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

// Copyright (C) 2009 Gaz Davidson
// Released under The Irrlicht Engine License, see irrlicht.h

#ifndef __C_BEAM_SCENE_NODE_H_INCLUDED__
#define __C_BEAM_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "S3DVertex.h"

namespace irr
{
namespace scene
{

//! Scene node  
class CBeamSceneNode : public ISceneNode
{
public:

	//! constructor
	CBeamSceneNode(ISceneNode* const parent, ISceneManager* const mgr, const s32 id,
			const core::vector3df& startPosition, const core::vector3df& endPosition, const f32 size);

	//! pre render event
	void OnRegisterSceneNode();

	//! render
	void render();

	//! returns the axis aligned bounding box of this node
	const core::aabbox3d<f32>& getBoundingBox() const;

	//! sets the size of the billboard
	void setSize(f32 size);

	//! gets the size of the billboard
	f32 getSize() const;

	//! sets the position of start and end points
	void setPosition(const core::vector3df& startPos, const core::vector3df& endPos);

	//! Returns the material
	video::SMaterial& getMaterial(u32 i);
	
	//! returns amount of materials used by this scene node.
	u32 getMaterialCount() const;
	


	//! Writes attributes of the scene node.
	void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

	//! Reads attributes of the scene node.
	void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	//! Returns type of the scene node
	ESCENE_NODE_TYPE getType() const { return ESCENE_NODE_TYPE(MAKE_IRR_ID('B','E','A','M')); }

	//! Creates a clone of this scene node and its children.
	//ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

private:

	void recalculateBoundingBox();

	f32 Size;

	core::vector3df EndPos;

	core::aabbox3d<f32> BBox;

	video::SMaterial Material;

	video::S3DVertex vertices[12];

	u16 indices[18];
};

} // end namespace scene
} // end namespace irr

#endif
