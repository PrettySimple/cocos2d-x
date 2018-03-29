//
//  CCInjectMouseMove-emscripten.h
//
#pragma once

/*

This class serves two purposes:

	1) Record the latest mouse position, which is required in couple of places (eg. the mouse wheel events don't hold the current position)
	2) Re-inject the latest mouse position every once a while, thus providing a quick (& dirty) solution to the updates happening in the scene.
		This allows us to handle scene updates (eg. a button/sprite that ended being under/off the cursor, or got enabled/disabled) in a fairly easy way.

*/

#include "2d/CCNode.h"
#include "base/CCEventMouse.h"


NS_CC_BEGIN


class CC_DLL InjectMouseMove : public Node
{
	friend class GLViewImpl;

	private:

		float	_designX;
		float	_designY;
		bool	_injecting;

	private:

		InjectMouseMove();
		InjectMouseMove(const InjectMouseMove&) = delete;
		InjectMouseMove& operator=(const InjectMouseMove&) = delete;
		InjectMouseMove(InjectMouseMove&&) noexcept = delete;
		InjectMouseMove& operator=(InjectMouseMove&&) noexcept = delete;
		~InjectMouseMove();

	// Making methods public as some are invoked from GLViewImpl's extern "C" functions.
	public:

		void	updatePosition(float designX, float designY);
		bool	getLastKnownPosition(float& designX, float& designY) const noexcept;

		void	pauseInject();

	private:
		void	_inject(float);
};


NS_CC_END
