#include "DrawThread.h"
#include "GuiMain.h"
#include <iostream>
#include <d3d11.h>
#include "stb_image.h"
#include "myImages.h"
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#define IM_PI 3.14159265358979323846f

extern ID3D11Device* g_pd3dDevice;

void DrawThread:: operator()(CommonObjects& common) {
    GuiMain(DrawAppWindow, &common, this);
    common.exit_flag = true;
}
void DrawAppWindow(void* common_ptr,void* callerPtr) {
    auto common = static_cast<CommonObjects*>(common_ptr);
    auto draw_thread = (DrawThread*)callerPtr;
    draw_thread->RenderBackgroundImage(common);
    draw_thread->RenderSearchBar(common);
    if (common->job_page_ready) 
    {
		/*draw_thread->current_jobs = common->jobs;*/
        draw_thread->current_jobs.insert(draw_thread->current_jobs.end(), common->jobs.begin(), common->jobs.end());
		common->job_page_ready = false;
		draw_thread->show_jobs_list = true;
    }
    if(draw_thread->show_jobs_list) draw_thread->display_jobs(common);
    if (common->stats_data_ready) {
        draw_thread->show_last_year_stats = true;   
		common->stats_data_ready = false;
    }
	if (draw_thread->show_last_year_stats) draw_thread->display_last_year_stats(*common);
    
}

void DrawThread:: RenderBackgroundImage(CommonObjects* common) {
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

}

void DrawThread:: RenderSearchBar(CommonObjects* common) {

    ImVec2 window_size = ImGui::GetIO().DisplaySize;
    float searchbar_width = 960.0f;
    float center_position = (window_size.x - searchbar_width) * 0.5f;
    float searchbar_y_pos = 270.0f;


    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(window_size);
    ImGui::Begin("SearchBarOverlay", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoSavedSettings);

    // Style variables
    const ImVec4 white_bg = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    const ImVec4 orange_button = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
    const ImVec4 black_text = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Static variables for combo boxes
    static int selected_job_type = -1;
    static int selected_location = -1;
    static int selected_sorte = -1;
    static int selected_field = -1;

    // Define options for each combo box
    static const char* locations[] = {
    "United Kingdom", "United States", "Austria", "Australia", "Belgium", 
    "Brazil", "Canada", "Switzerland", "Germany", "Spain", "France", 
    "India", "Italy", "Mexico", "Netherlands", "New Zealand", "Poland", 
    "Singapore", "South Africa"
    };
    static const char* fields[] = { "accounting-finance", "it", "sales", "customer-services", "engineering", "hr",
        "healthcare-nursing", "hospitality-catering", "pr-advertising-marketing", "logistics-warehouse",
        "teaching", "trade-construction", "admin", "legal", "creative-design", "graduate", "retail",
        "consultancy", "manufacturing", "scientific-qa", "social-work", "travel", "energy-oil-gas",
        "property", "charity-voluntary", "domestic-help-cleaning", "maintenance", "part-time",
        "other-general" 
    };
    static const char* job_types[] = {"All", "Full Time", "Part Time"};
    static const char* sorted_by[] = {"Date", "Salary", "Relevance"};


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
    
    ImGui::SetCursorPosX(center_position);
    ImGui::SetCursorPosY(searchbar_y_pos); 


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
  
    RenderCustomComboBox("Field", fields, IM_ARRAYSIZE(fields), &selected_field, column_width);

    // Field Column
    ImGui::NextColumn();

    ImGui::SetColumnWidth(2, column_width);
    RenderCustomComboBox("Job Type", job_types, IM_ARRAYSIZE(job_types), &selected_job_type, column_width);

    // Role Column
    ImGui::NextColumn();
    ImGui::SetColumnWidth(3, column_width);
    RenderCustomComboBox("Sorted by", sorted_by, IM_ARRAYSIZE(sorted_by), &selected_sorte, column_width);

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
        if (selected_job_type != -1 && selected_sorte != -1 && selected_field != -1 && selected_location != -1) {
            
            common->country = country_codes.at(locations[selected_location]);
			common->field = fields[selected_field];
			common->job_type = job_types[selected_job_type];
            common->sorted_by = sorted_by[selected_sorte];
			current_jobs.clear();
            common->start_job_searching=true;
            common->cv.notify_one();  

        }
    }
   
    ImGui::EndGroup();
    ImGui::EndChild();

    // Pop all style modifications
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(6);
    ImGui::End();

}

void DrawThread::RenderCustomComboBox(const char* label, const char* items[], size_t items_count, int* selected_item, float column_width) {

    ImVec2 screenPos = ImGui::GetCursorScreenPos();

    // Get the current cursor position relative to the window
    ImVec2 pos = ImGui::GetCursorPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImGui::PushID(label);
    float searchbar_height = 80.0f;
    float padding_top = 10.0f;

    // Calculate label position
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float labelOffsetX = (column_width - labelSize.x) * 0.5f;

    const char* preview = (*selected_item >= 0) ? items[*selected_item] : "Choose...";
    ImVec2 previewSize = ImGui::CalcTextSize(preview);
    float previewOffsetX = (column_width - previewSize.x) * 0.5f;

    ImGui::BeginGroup();

    ImGui::SetCursorPos(pos);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.85f, 0.85f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.75f, 0.75f, 0.5f));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    bool combo_clicked = ImGui::Button("##combo", ImVec2(column_width - 10, 65.0f));

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);

    // Set relative position for the label
    ImGui::SetCursorPos(ImVec2(pos.x + labelOffsetX, pos.y + padding_top));

    // Add some contrast to make the text visible
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(pos.x + previewOffsetX, pos.y + padding_top + labelSize.y + 2));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::Text("%s", preview);
    ImGui::PopStyleColor();


    if (label != "Location") {
        ImGui::SameLine();
        draw_list->AddLine(
            ImVec2(screenPos.x - 5, screenPos.y + 5),
            ImVec2(screenPos.x - 5, screenPos.y + 60),
            IM_COL32(211, 211, 211, 255),
            1.0f
        );
    }

    if (combo_clicked) ImGui::OpenPopup(label);
    ImGui::SetNextWindowPos(ImVec2(screenPos.x, screenPos.y + 70));

    // Calculate the height needed for all items
    float item_height = ImGui::GetTextLineHeightWithSpacing();
    float content_height = item_height * items_count;
    float popup_height = content_height + ImGui::GetStyle().WindowPadding.y * 2;
    if (popup_height > 200.0f) popup_height = 200.0f;

    ImGui::SetNextWindowSize(ImVec2(220.0f, popup_height));

    if (ImGui::BeginPopup(label, ImGuiWindowFlags_NoMove)) {

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f); 

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false);

        for (int i = 0; i < items_count; i++) {
            if (ImGui::Selectable(items[i], i == *selected_item)) {
                *selected_item = i;
                ImGui::CloseCurrentPopup(); // Close popup when item is selected
            }
        }
        ImGui::PopStyleVar();

        ImGui::EndChild();
        ImGui::EndPopup();
    }

    ImGui::EndGroup();
    ImGui::PopID();
}

void DrawThread::display_jobs(CommonObjects* common)
{
    ImVec2 mainViewportSize = ImGui::GetMainViewport()->Size;
    ImVec2 windowSize = ImVec2(mainViewportSize.x * 0.8f, mainViewportSize.y * 0.8f);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);
    ImGui::Begin("Job Listings", &show_jobs_list);

    // Add a table with 6 columns (including the new Star column)
    if (ImGui::BeginTable("JobTable", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
    {
        // Table headers
        ImGui::TableSetupColumn("num", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Company", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Salary", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableHeadersRow();

        // Iterate over the jobs and create rows in the table
        for (size_t i = 0; i < current_jobs.size(); ++i)
        {
            Jobs& job = current_jobs[i];
            ImGui::TableNextRow();
            // Display job number
            ImGui::TableNextColumn();
            if (ImGui::TreeNodeEx((std::to_string(i + 1)).c_str(), job.is_expanded ? ImGuiTreeNodeFlags_DefaultOpen : 0))
            {
                ImGui::TableNextColumn();
                ImGui::Text("%s", job.title.c_str());
                ImGui::TextWrapped("Description: %s", job.description.c_str());
                ImGui::Text("Posted on: %s", job.created_date.c_str());
                ImGui::Text("URL: %s", job.url.c_str());
                ImGui::TreePop();
                job.is_expanded = true;
            }
            else
            {
                job.is_expanded = false;
                ImGui::TableNextColumn();
                ImGui::Text("%s", job.title.c_str());
            }

            // Display the rest of the columns
            ImGui::TableNextColumn();
            ImGui::Text("%s", job.company.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", job.location.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", job.salary.c_str());
            // Star column
            ImGui::TableNextColumn();
            std::string star_id = "star_button_" + std::to_string(i+1);
            StarButton(star_id.c_str(), job.is_starred);

        }

        ImGui::EndTable();
    }

    // Calculate the button position
    float window_width = ImGui::GetWindowSize().x;
    float button_width = 120.0f; // Width of the button
    float button_x = (window_width - button_width) * 0.5f; // Center the button horizontally
    ImGui::SetCursorPosX(button_x);

    // Add button to load more jobs
    if (jobsButton("More Jobs",button_width))
    {
        common->current_page++;
        common->start_job_searching = true;
        common->cv.notify_one();
    }
	ImGui::SameLine();
    if (jobsButton("Salary data", button_width))
    {
		common->download_jobs_stats = true;
		common->cv.notify_one();
    }
    ImGui::SameLine();
	if (jobsButton("save to file", button_width)) saveStarredJobsToFile();
    ImGui::End();
}

bool DrawThread::StarButton(const char* id, bool& is_starred)
{
    ImGui::PushID(id);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    float size = ImGui::GetFrameHeight();
    ImVec2 center = ImVec2(p.x + size * 0.5f, p.y + size * 0.5f);

    ImGui::InvisibleButton("star_button", ImVec2(size, size));
    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();

    if (clicked)
        is_starred = !is_starred; // Toggle the state on click

    // Choose color: yellow for starred, white otherwise
    ImU32 color = is_starred ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 255, 255, 255);

    // Draw the star with consistent size and radius
    DrawStar(draw_list, center, size * 0.4f, color, is_starred);

    ImGui::PopID();
    return clicked;
}

void DrawThread:: DrawStar(ImDrawList* draw_list, ImVec2 center, float radius, ImU32 color, bool filled) {
    const int num_points = 5;
    ImVec2 points[num_points * 2];
    float start_angle = -IM_PI / 2; // Start with the top point of the star
    float angle_increment = IM_PI / num_points;

    for (int i = 0; i < num_points * 2; i++) {
        // Use consistent radii for all stars
        float r = (i % 2 == 0) ? radius : radius * 0.45f;
        float angle = start_angle + i * angle_increment;
        points[i] = ImVec2(center.x + r * cosf(angle), center.y + r * sinf(angle));
    }

    // Fill the star only if `filled` is true
    if (filled) {
        draw_list->AddConvexPolyFilled(points, num_points * 2, color); // Fill inside the star
    }

    // Draw the outer border on top to ensure it stays visible
    draw_list->AddPolyline(points, num_points * 2, IM_COL32(255, 255, 255, 255), true, 2.0f);
}

bool DrawThread:: jobsButton(const char* label,float button_width)
{
	ImGui::PushID(label);
	ImVec2 button_size = ImVec2(button_width, 30);
	ImVec2 p = ImGui::GetCursorScreenPos();
	bool is_clicked = ImGui::InvisibleButton(label, button_size);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImU32 color_bg    = ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15, 1.0f));
    ImU32 color_hover = ImGui::GetColorU32(ImVec4(0.25f, 0.25f, 0.25, 1.0f));
    ImU32 color_g1    = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
    ImU32 color_g2    = ImGui::GetColorU32(ImVec4(1.0f, 0.15f, 0.15f, 1.0f));
    ImU32 color_text  = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); 

	draw_list->AddRectFilledMultiColor(p, ImVec2(p.x + button_size.x, p.y + button_size.y), color_g1, color_g2, color_g2, color_g1);
    if (ImGui::IsItemHovered()) draw_list->AddRectFilled(p, ImVec2(p.x + button_size.x, p.y + button_size.y), color_hover);
	ImVec2 text_size = ImGui::CalcTextSize(label);
	ImVec2 text_pos = ImVec2(p.x + (button_size.x - text_size.x) * 0.5f, p.y + (button_size.y - text_size.y) * 0.5f);
	draw_list->AddText(text_pos, color_text, label);
	ImGui::PopID();
    return is_clicked;
}

void DrawThread::display_last_year_stats(CommonObjects& common) {
    ImGui::Begin("Salary Data", &show_last_year_stats);
    ImPlot::CreateContext();
    float months[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    std::string title = "Average Salary Over 2024 in field: " + common.field;
    if (ImPlot::BeginPlot(title.c_str())) {
        ImPlot::SetupAxes("Month", "Average Salary ($)");
        ImPlot::PlotLine("Salary", months, common.salaries, 12);
        ImPlot::EndPlot();
    }
    ImGui::End();
}

void DrawThread::saveStarredJobsToFile()
{
    const std::string default_file_name = "starred_jobs.txt";
    std::ofstream out_file;
    if (!starred_file_exists) {
        out_file.open(default_file_name, std::ios::app);
        if (!out_file) {
            std::cerr << "Error: Unable to open or create the file: " << default_file_name << std::endl;
            return;
        }
        starred_file_exists = true;
    }
    else {
        out_file.open(default_file_name, std::ios::app);
        if (!out_file) {
            std::cerr << "Error: Unable to append to the file: " << default_file_name << std::endl;
            return;
        }
    }
    for (const auto& job : current_jobs) {
        if (job.is_starred) {
            out_file << "Title: " << job.title << "\n";
            out_file << "Company: " << job.company << "\n";
            out_file << "Location: " << job.location << "\n";
            out_file << "URL: " << job.url << "\n";
            out_file << "Salary: " << job.salary << "\n";
            out_file << "Description: " << job.description << "\n";
            out_file << "Created Date: " << job.created_date << "\n";
            out_file << "-------------------------------------------\n";
        }
    }
    out_file.close();
    std::cout << "Starred jobs saved successfully to: " << default_file_name << std::endl;
}

/*
*  This code creates a texture from an image that is in the memory!
*  It use the stb_image library to load the image from memory.
*  The image is in the form of an unsigned char array in myImages.h file.
*/ 
ID3D11ShaderResourceView* DrawThread:: CreateTextureFromImage(const unsigned char* image_data, int width, int height, int channels)
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


