/*
 * Flocking demo for the Irrlicht In Motion Competition 08/09 by randomMesh
 *
 * Boids, developed by Craig Reynolds (http://www.red3d.com/cwr/) in 1986, is an artificial life program,
 * simulating the flocking behaviour of birds.
 *
 * This demo is an implementation of the pseudocode found at the website of Conrad Parker.
 * See http://www.vergenet.net/~conrad/boids/pseudocode.html
 *
 *
 *
 * Used libraries:
 *
 * 3D Engine - Irrlicht SVN trunk
 * Soundengine - irrKlang 1.2.0
 *
 *
 * Symbols:
 *
 * -D_SOUND - Add _SOUND to the defined symbols to compile with sound using irrklang
 *
 *
 * License:
 *
 * The Irrlicht Engine License
 *
 *  Copyright © 2008-2009 randomMesh
 *
 *	This software is provided 'as-is', without any express or implied warranty.
 *	In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *	Permission is granted to anyone to use this software for any purpose,
 *	including commercial applications, and to alter it and redistribute it freely,
 *	including subject to the following restrictions:
 *
 *		The origin of this software must not be misrepresented;
 *		you must not claim that you wrote the original software.
 *		If you use this software in a product, an acknowledgment in the product documentation
 *		would be appreciated but is not required.
 *
 *		Altered source versions must be plainly marked as such, and must not be misrepresented
 *		as being the original software.
 *
 *		This notice may not be removed or altered from any source distribution.
 */

#include "core/Application.h"

int main(int argc, char* argv[])
{
	Application app;

	if (app.init(argc, argv))
		app.run();

	return 0;
}
