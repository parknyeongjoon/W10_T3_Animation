#pragma once

class IWindowToggleable
{
public:
	virtual void Toggle() = 0;

	virtual ~IWindowToggleable() {}
};
