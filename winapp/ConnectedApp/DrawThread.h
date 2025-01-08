#pragma once
#include "CommonObject.h"
#include <d3d11.h>
class DrawThread
{
public:
	void operator()(CommonObjects& common);
private:
    const std::unordered_map<std::string, std::string> country_codes = {
        {"United Kingdom", "gb"},
        {"United States", "us"},
        {"Austria", "at"},
        {"Australia", "au"},
        {"Belgium", "be"},
        {"Brazil", "br"},
        {"Canada", "ca"},
        {"Switzerland", "ch"},
        {"Germany", "de"},
        {"Spain", "es"},
        {"France", "fr"},
        {"India", "in"},
        {"Italy", "it"},
        {"Mexico", "mx"},
        {"Netherlands", "nl"},
        {"New Zealand", "nz"},
        {"Poland", "pl"},
        {"Singapore", "sg"},
        {"South Africa", "za"}
    };
};
void DrawAppWindow(void* common_ptr);
void RenderSearchBar();
void RenderBackgroundImage(CommonObjects* common);
void RenderCustomComboBox(const char* label, const char* items[], int items_count, int* selected_item, float column_width);
ID3D11ShaderResourceView* CreateTextureFromImage(const unsigned char* image_data, int width, int height, int channels);

