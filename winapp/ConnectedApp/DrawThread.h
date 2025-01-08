#pragma once
#include "CommonObject.h"
#include <d3d11.h>
class DrawThread
{
public:
	void operator()(CommonObjects& common);
    
};

