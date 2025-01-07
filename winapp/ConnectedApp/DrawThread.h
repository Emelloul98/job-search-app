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
void RenderBackgroundImage(CommonObjects* common);
void RenderCustomComboBox(const char* label, const char* items[], int items_count, int* selected_item, float column_width);
ID3D11ShaderResourceView* CreateTextureFromImage(const unsigned char* image_data, int width, int height, int channels);

