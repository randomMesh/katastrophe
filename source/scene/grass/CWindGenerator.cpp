// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#include "CWindGenerator.h"

namespace irr
{
namespace scene
{

const core::vector2df& CWindGenerator::getWind(const core::vector3df& position, const u32 timeMs)
{
	const f32 seed = (timeMs + position.X*7*cos(timeMs/120000.0f) + position.Z*7*sin(timeMs/120000.0f))/1000.0f;
	const f32 dir = 2*core::PI*noise(seed/Regularity);
	const f32 amp = Strength*sin(seed);

	this->wind.X = amp*cos(dir);
	this->wind.Y = amp*sin(dir);

	return this->wind;
}

f32 CWindGenerator::noise(const f32 x) const
{
	f32 total = 0;
	f32 frequency = 1.0f;
	f32 amplitude = 1.0f;

	for (irr::u32 i = 0; i < 4; ++i)
	{
		total += noiseInterpolate(x*frequency)*amplitude;

		frequency = frequency + frequency;
		amplitude *= 0.50f;
	}

	return total;
}

f32 CWindGenerator::noiseInterpolate(const f32 x) const
{
	const s32 intX = (int)(x);
	const f32 fracX = x - intX;

	return cosInterpolater(windSmoother(intX), windSmoother(intX + 1), fracX);
}

f32 CWindGenerator::windSmoother(const s32 x) const
{
	return rndGenerator(x)/2  +  rndGenerator(x-1)/4  +  rndGenerator(x+1)/4;
}

f32 CWindGenerator::cosInterpolater(const f32 a, const f32 b, const f32 x) const
{
	const f32 ft = x*irr::core::PI;
	const f32 f = (1 - cos(ft))*0.5f;

	return a*(1 - f) + b*f;
}

f32 CWindGenerator::rndGenerator(s32 x) const
{
	x = (x<<13) ^ x;
	return ( (f32)(1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0));
}

}
}
