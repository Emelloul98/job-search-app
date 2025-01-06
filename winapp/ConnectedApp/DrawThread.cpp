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
void RenderCustomComboBox(const char* label, const char* items[], int items_count, int* selected_item, float column_width) {
    ImGui::PushID(label);

    float searchbar_height = 80.0f;
    float combo_height = ImGui::GetFrameHeight() * 2 + 4;
    float padding_top = (searchbar_height - combo_height) * 0.5f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padding_top);

    ImGui::BeginGroup();

    ImVec2 rect_min = ImGui::GetCursorScreenPos();
    float actual_width = column_width - 30;
    ImVec2 rect_max = ImVec2(rect_min.x + actual_width, rect_min.y + combo_height);
    float content_start_x = rect_min.x + actual_width * 0.3f;

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::SetCursorScreenPos(ImVec2(content_start_x, rect_min.y));
    ImGui::Text("%s", label);
    ImGui::PopFont();


    const char* preview = (*selected_item >= 0) ? items[*selected_item] : "Choose...";
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::SetCursorScreenPos(ImVec2(content_start_x, rect_min.y + ImGui::GetFrameHeight() + 2));
    ImGui::Text("%s", preview);
    ImGui::PopStyleColor();

    ImVec2 button_rect_min = ImVec2(rect_min.x, rect_min.y - (padding_top*2.0f));
    ImVec2 button_rect_max = ImVec2(button_rect_min.x + actual_width, button_rect_min.y + combo_height);

    ImGui::SetCursorScreenPos(ImVec2(button_rect_min.x, button_rect_min.y));
    bool combo_clicked = ImGui::InvisibleButton("##combo", ImVec2(actual_width, combo_height));

    ImU32 rect_color = ImGui::GetColorU32(
        ImGui::IsItemActive() ? ImVec4(0.75f, 0.75f, 0.75f, 0.5f) :
        ImGui::IsItemHovered() ? ImVec4(0.85f, 0.85f, 0.85f, 0.5f) :
        ImVec4(0.9f, 0.9f, 0.9f, 0.0f)
    );

    ImGui::GetWindowDrawList()->AddRectFilled(button_rect_min, button_rect_max, rect_color, 30.0f);  

    if (label != "Role") {
        ImGui::SameLine();
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(ImGui::GetCursorScreenPos().x + 7, ImGui::GetCursorScreenPos().y + 12),
            ImVec2(ImGui::GetCursorScreenPos().x + 7, ImGui::GetCursorScreenPos().y + 64),
            IM_COL32(211, 211, 211, 255),  // Light gray
            1.0f
        );
    }

    if (combo_clicked) ImGui::OpenPopup("##popup");

    ImGui::SetNextWindowPos(ImVec2(rect_min.x, rect_max.y));
    ImGui::SetNextWindowSize(ImVec2(actual_width, 0));

    if (ImGui::BeginPopup("##popup", ImGuiWindowFlags_NoMove)) {
        for (int i = 0; i < items_count; i++) {
            ImGui::SetCursorPosX(content_start_x - rect_min.x);
            if (ImGui::Selectable(items[i], i == *selected_item)) {
                *selected_item = i;
            }
        }
        ImGui::EndPopup();
    }

    ImGui::EndGroup();
    ImGui::PopID();
}
void RenderSearchBar(bool* is_clicked) {
    // Style variables
    const ImVec4 white_bg = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    const ImVec4 orange_button = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
    const ImVec4 black_text = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Static variables for combo boxes
    static int selected_job = -1;
    static int selected_location = -1;
    static int selected_role = -1;
    static int selected_field = -1;

    // Define options for each combo box
    static const char* job_types[] = { "Full Time", "Part Time", "Freelance", "Internship", "Contract" };
    static const char* locations[] = { "New York", "London", "Tokyo", "Berlin", "Remote" };
    static const char* roles[] = { "Developer", "Designer", "Manager", "Analyst", "Engineer" };
    static const char* fields[] = { "Technology", "Healthcare", "Finance", "Education", "Marketing" };

    // Set up the main container style
    ImGui::PushStyleColor(ImGuiCol_ChildBg, white_bg);
    ImGui::PushStyleColor(ImGuiCol_Button, orange_button);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.6f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, black_text);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, white_bg);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, white_bg);

    // Remove frame and round edges
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 25.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 8));

    // Center the SearchBar
    float window_width = ImGui::GetWindowSize().x;
    float searchbar_width = 960.0f;
    //float searchbar_height = 80; 
    float center_position = (window_width - searchbar_width) * 0.5f;
    ImGui::SetCursorPosX(center_position);


    // Begin main container
    ImGui::BeginChild("SearchBar", ImVec2(searchbar_width, 80), true,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);

    ImGui::BeginGroup();
    ImGui::Columns(5, nullptr, false);

    float column_width = 220;
    float button_column_width = 60;  // Width for the button column

    // Location Column
    ImGui::SetColumnWidth(0, column_width);
    RenderCustomComboBox("Location", locations, IM_ARRAYSIZE(locations), &selected_location, column_width);


    // Job Type Column
    ImGui::NextColumn();
    ImGui::SetColumnWidth(1, column_width);
    RenderCustomComboBox("Job Type", job_types, IM_ARRAYSIZE(job_types), &selected_job, column_width);

    // Field Column
    ImGui::NextColumn();
    ImGui::SetColumnWidth(2, column_width);
    RenderCustomComboBox("Field", fields, IM_ARRAYSIZE(fields), &selected_field, column_width);

    // Role Column
    ImGui::NextColumn();
    ImGui::SetColumnWidth(3, column_width);
    RenderCustomComboBox("Role", roles, IM_ARRAYSIZE(roles), &selected_role, column_width);

    // Move button inside the group, after the last column
    ImGui::NextColumn();
    ImGui::SetColumnWidth(4, button_column_width);
    float button_size = 40.0f;
    float searchbar_height = 80.0f;
    float button_y = (searchbar_height - button_size) * 0.5f;

    ImGui::PopStyleVar(3);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button_size / 2.0f); 

    ImGui::SetCursorPosY(button_y); // Only adjust Y position
    if (ImGui::Button("S", ImVec2(button_size, button_size))) {
        // Handle search action
    }
    ImGui::EndGroup();
    ImGui::EndChild();

    // Pop all style modifications
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(6);
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


