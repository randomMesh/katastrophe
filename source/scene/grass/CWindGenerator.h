// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

/*
  CWindGenerator.h - thank you Klasker :)
  http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=10762&start=15
 */

#include "IWindGenerator.h"

namespace irr
{
namespace scene
{

class CWindGenerator : public IWindGenerator
{

public:

	const core::vector2df& getWind(const core::vector3df& position, const u32 timeMs);

	inline f32 getStrength() const { return Strength; }

	inline f32 getRegularity() const { return Regularity; }

	inline void setStrength(const f32 strength) { Strength = strength; }

	inline void setRegularity(const f32 regularity) { Regularity = regularity; }

private:

	f32 rndGenerator(s32 x) const;

	f32 cosInterpolater(const f32 a, const f32 b, const f32 x) const;

	f32 windSmoother(const s32 x) const;

	f32 noiseInterpolate(const f32 x) const;

	f32 noise(const f32 x) const;

	f32 Strength;

	f32 Regularity;
};

} // namespace scene
} // namespace irr
