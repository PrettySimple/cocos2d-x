//
//  CCInjectMouseMove-emscripten.h
//
#pragma once

/*

See comments in CCGLViewImpl-emscripten.cpp

*/

#include "2d/CCNode.h"
#include "base/CCEventMouse.h"


NS_CC_BEGIN


class CC_DLL InjectMouseMove : public Node
{
	friend class GLViewImpl;

	private:

		// Oops, reusing the event didn't work out well - it seems Cocos internally uses EventMouse::_startPoint/EventMouse::_prevPoint...
		//EventMouse	_event;
		// Instead, we're now recreating an EventMouse on every inject
		float	_designX;
		float	_designY;

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
		void	pauseInject();

	private:
		void	_inject(float);
};


NS_CC_END
