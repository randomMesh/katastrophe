// Copyright (C) 2009 randomMesh
// This file is part of the "Flocking boids" demo.
// For conditions of distribution and use, see copyright notice in main.cpp

#ifndef _T_STATE_H_INCLUDED__
#define _T_STATE_H_INCLUDED__

/*! \class StateT
 *
 *  This is the state base class.
 *  All states that are used with the state machine will inherit from this state, and the template parameters for
 *  the state and the state graph [machine] will be the same.
 *
 * Code is originally from vitek. See http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=19949
 */
template <class T, class E>
class State
{

public:

	virtual ~State()
	{
	}

	/*! Enter this state.
	 *
	 * Called by the state machine when entity is entering this state.
	 *
	 * \param entity The entity that is entering this state.
	 */
	virtual void onEnter (T* const entity)
	{
	}

	/*! Leave this state.
	 *
	 * Called by the state machine when entity is leaving this state.
	 *
	 * \param entity The entity that is leaving this state.
	 */
	virtual void onLeave (T* const entity)
	{
	}

	/*! Update the entity state.
	 *
	 * \param entity The entity that is to be updated.
	 */
	virtual void onUpdate(T* const entity)
	{
	}

	/*! Process a message.
	 *
	 * Called by the state machine when a message is to be processed for
	 * an entity.
	 *
	 * \param entity The entity that message is destined for.
	 * \param event The event to process.
	 * \return Returns true if the message was processed, otherwise returns
	 * false.
	 */
	virtual const bool onEvent(T* const entity, const E& event)
	{
		return false;
	}

	/*! Render to texture
	 *
	 * Called by the state machine between driver->beginScene() and smgr->drawAll().
	 *
	 * \param entity The entity that should draw.
	 */
	virtual void rttCallback(T* const entity) const
	{
	}

	/*! Draw to Irrlicht scene
	 *
	 * Called by the state machine between smgr->drawAll() and guienv->drawAll().
	 *
	 * \param entity The entity that should draw.
	 */
	virtual void drawCallback(T* const entity) const
	{
	}
};

#endif // _T_STATE_H_INCLUDED__
