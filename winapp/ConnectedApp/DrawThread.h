#pragma once
#include "CommonObject.h"
#include <d3d11.h>
class DrawThread
{
public:
	void operator()(CommonObjects& common);

};

void DrawAppWindow(void* common_ptr);
void RenderSearchBar();
void RenderBackgroundAndButton(CommonObjects* common);
ID3D11ShaderResourceView* CreateTextureFromImage(const unsigned char* image_data, int width, int height, int channels);

