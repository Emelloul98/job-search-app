#pragma once
using drawcallback = void(void*);

int GuiMain(drawcallback drawfunction, void* obj_ptr);
bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
extern ID3D11Device* g_pd3dDevice;

