#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <iostream>
#include <d3d11.h>
#include "stb_image.h"
#include "myImages.h"

extern ID3D11Device* g_pd3dDevice;
void DrawThread::operator()(CommonObjects& common) {
    GuiMain(DrawAppWindow, &common);
    common.exit_flag = true;
}
void DrawAppWindow(void* common_ptr) {
    auto common = static_cast<CommonObjects*>(common_ptr);
    RenderBackgroundAndButton(common);
    if (common->find_job_button_clicked) RenderSearchBar(&common->find_job_button_clicked);
}
void RenderBackgroundAndButton(CommonObjects* common) {
    /*
        Drawing the background image
    */
    int image_width, image_height, channels;
    unsigned char* background_img_data = stbi_load_from_memory(image_data, sizeof(image_data), &image_width, &image_height, &channels, 4);
    ID3D11ShaderResourceView* my_texture = CreateTextureFromImage(background_img_data, image_width, image_height, channels);
    stbi_image_free(background_img_data);

    // Get the size of the window
    ImVec2 window_size = ImGui::GetIO().DisplaySize;

    // Draw the background image
    ImGui::GetBackgroundDrawList()->AddImage(my_texture,
        ImVec2(0, 0),  // Start at top-left corner of the window
        ImVec2(window_size.x, window_size.y),  // End at bottom-right corner of the window
        ImVec2(0, 0),  // Tex coords (start)
        ImVec2(1, 1)   // Tex coords (end)
    );

    /*
        Adds a button to the background image
    */
    ImVec2 button_size(300, 50);
    ImVec2 button_pos((window_size.x - button_size.x) * 0.48f, (window_size.y + button_size.y) * 0.8f);
    // Hover effect: Change color when hovering
    ImColor button_color = IM_COL32(0, 128, 255, 255);  // Default color (blue)
    if (ImGui::IsMouseHoveringRect(button_pos, ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y))) {
        button_color = IM_COL32(0, 150, 255, 255);  // Light blue color when hovering
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    // Click effect: Darker color when clicked
    if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(button_pos, ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y))) {
        button_color = IM_COL32(0, 100, 200, 255);
        common->find_job_button_clicked = true;
    }
    // Draw the button background
    ImGui::GetBackgroundDrawList()->AddRectFilled(button_pos, ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y), button_color);
    // Calculate text size and position to center the text
    const char* button_text = "Find your dream job";
    ImVec2 text_size = ImGui::CalcTextSize(button_text);
    ImVec2 text_pos(button_pos.x + (button_size.x - text_size.x) * 0.5f, button_pos.y + (button_size.y - text_size.y) * 0.5f);
    // Draw the centered text
    ImGui::GetBackgroundDrawList()->AddText(text_pos, IM_COL32(255, 255, 255, 255), button_text);
}
void RenderSearchBar(bool* is_clicked) {
    static const char* countries[] = { "gb", "us", "at", "au" , "be" , "br" , "ca" , "ch" , "de" , "es" , "fr" , "in" , "it" , "mx" , "nl", "nz" , "pl", "sg" , "za" };
    static int selected_country = 0; 
    static char job_type[64] = "";  
    static char location[64] = ""; 
    static char role[64] = "";     
    static char field[64] = "";    
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f); 
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10)); 
    ImGui::BeginChild("SearchBar", ImVec2(800, 100), *is_clicked, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);
    ImGui::Columns(5, nullptr, false); 
    ImGui::SetColumnWidth(0, 80);
    if (ImGui::Button("Search", ImVec2(60, 60))) {

    }
    // Job Type ComboBox
    ImGui::NextColumn();
    ImGui::Text("Job Type");
    if (ImGui::BeginCombo("##JobType", countries[selected_country])) {
        for (int i = 0; i < IM_ARRAYSIZE(countries); i++) {
            bool is_selected = (selected_country == i);
            if (ImGui::Selectable(countries[i], is_selected)) {
                selected_country = i; // Update selected job type
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus(); // Set focus on the selected item
            }
        }
        ImGui::EndCombo();
    }
    ImGui::NextColumn();
    ImGui::Text("Location");
    ImGui::InputText("##Location", location, IM_ARRAYSIZE(location));
    ImGui::NextColumn();
    ImGui::Text("Role");
    ImGui::InputText("##Role", role, IM_ARRAYSIZE(role));
    ImGui::NextColumn();
    ImGui::Text("Field");
    ImGui::InputText("##Field", field, IM_ARRAYSIZE(field));
    ImGui::EndChild();
    ImGui::PopStyleVar(2); 
}

/*
*  This code creates a texture from an image that is in the memory!
*  It use the stb_image library to load the image from memory.
*  The image is in the form of an unsigned char array in myImages.h file.
*/ 
ID3D11ShaderResourceView* CreateTextureFromImage(const unsigned char* image_data, int width, int height, int channels) 
{
    ID3D11ShaderResourceView* texture_view = nullptr;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = width * 4;

    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &texture);

    if (SUCCEEDED(hr)) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        hr = g_pd3dDevice->CreateShaderResourceView(texture, &srvDesc, &texture_view);
        texture->Release();
    }

    return texture_view;
}


