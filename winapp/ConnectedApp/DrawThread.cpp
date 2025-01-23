#pragma once
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
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
#define IM_PI 3.14159265358979323846f

void DrawThread:: operator()(CommonObjects& common) {
    GuiMain(DrawAppWindow, &common, this);
    common.exit_flag = true;
}
void DrawAppWindow(void* common_ptr,void* callerPtr) {
    auto common = static_cast<CommonObjects*>(common_ptr);
    auto draw_thread = (DrawThread*)callerPtr;
	// image currently not working:
	if (draw_thread->texture == nullptr) draw_thread->InitializeTextures();
    draw_thread->RenderBackgroundImage(common);
    draw_thread->RenderSearchBar(common);
    if (common->job_page_ready) 
    {
		/*draw_thread->current_jobs = common->jobs;*/
        draw_thread->current_jobs.insert(draw_thread->current_jobs.end(), common->jobs.begin(), common->jobs.end());
		common->job_page_ready = false;
		draw_thread->show_jobs_list = true;
    }
    if(draw_thread->show_jobs_list)
        draw_thread->display_jobs(common);

}

void DrawThread::InitializeTextures() {
    int image_width = 0, image_height = 0;
    //bool success = LoadTextureFromFile("C:\\Users\\mello\\cppCource\\job-search-app\\images\\image3.jpg", &texture, &image_width, &image_height);    //bool success = LoadTextureFromFile("C:\\Users\\mello\\cppCource\\job-search-app\\images\\image3.jpg", &texture, &image_width, &image_height);
    bool success = LoadTextureFromFile("../../images/background.jpg", &texture, &image_width, &image_height);
    if (!success) {
        std::cerr << "Failed to load image!" << std::endl;
    }
}
void DrawThread::RenderBackgroundImage(CommonObjects* common) {
    // Get the window size
    ImVec2 window_size = ImGui::GetIO().DisplaySize;
    // Draw the background image
    ImGui::GetBackgroundDrawList()->AddImage(
        (void*)texture,
        ImVec2(0, 0),  // Start at top-left corner of the window
        ImVec2(window_size.x, window_size.y),  // End at bottom-right corner of the window
        ImVec2(0, 0),  // Texture coordinates (start)
        ImVec2(1, 1)   // Texture coordinates (end)
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
        ImGuiWindowFlags_NoSavedSettings);

    // Style variables
    const ImVec4 white_bg = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    const ImVec4 orange_button = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
    const ImVec4 black_text = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Static variables for combo boxes
    static int selected_job_type = -1;
    static int selected_location = -1;
    static int selected_days_old = -1;
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
    static const char* max_days_old[] = {"All", "10", "20", "30","60"};


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
    RenderCustomComboBox("Maximum days old", max_days_old, IM_ARRAYSIZE(max_days_old), &selected_days_old, column_width);

    // Move button inside the group, after the last column
    ImGui::NextColumn();
    ImGui::SetColumnWidth(4, button_column_width);
    float button_size = 40.0f;
    float searchbar_height = 80.0f;
    float button_y = (searchbar_height - button_size) * 0.5f;

    ImGui::PopStyleVar(3);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button_size / 2.0f); 

    ImGui::SetCursorPosY(button_y); // Only adjust Y position
    if (ImGui::Button(ICON_MAGNIFYING_GLASS, ImVec2(button_size, button_size))) {

        if (selected_job_type != -1 && selected_days_old != -1 && selected_field != -1 && selected_location != -1) {
            common->job_page_ready = false;
            common->stats_data_ready = false;
            common->companies_data_ready = false;
			common->current_page = 1;
			common->exit_flag = false;

            common->country = country_codes.at(locations[selected_location]);
			common->field = fields[selected_field];
			common->job_type = job_types[selected_job_type];
            common->max_days_old = max_days_old[selected_days_old];
			current_jobs.clear();
            common->start_job_searching=true;
            common->cv.notify_one();  
          
        }
    }
   
    ImGui::EndGroup();
    ImGui::EndChild();

    // Add Favorites button below the search bar
    const float favorites_button_width = 200.0f;  // Width of favorites button
    const float favorites_button_height = 50.0f;  // Height of favorites button
    const float favorites_y_offset = 20.0f;       // Space between searchbar and favorites button

     // Calculate position for centered favorites button
    float favorites_x = center_position + (searchbar_width - favorites_button_width) * 0.5f;
    float favorites_y = searchbar_y_pos + searchbar_height + favorites_y_offset;

    static bool favorite_jobs_is_open = false;

    // Style for favorites button
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    
    ImGui::SetCursorPos(ImVec2(favorites_x, favorites_y));

    if (ImGui::Button("Favorites "  ICON_FA_HEART, ImVec2(favorites_button_width, favorites_button_height))) {
		favorite_jobs_is_open = true;
        ImGui::OpenPopup("Favorite Jobs");
    }
    // Popup window


    if (ImGui::BeginPopupModal("Favorite Jobs", &favorite_jobs_is_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        std::unordered_map<std::string, Job> favoriteJobs  = common->favorite_jobs.getFavorites();

        // Add a little padding
        ImGui::BeginChild("ScrollingRegion", ImVec2(600, 400), true);

        for (const auto& [job_id, job] : favoriteJobs) {
			int i = 0;
            ImVec2 lineStart = ImGui::GetCursorPos();

            float windowWidth = ImGui::GetWindowWidth();
            ImGui::SetCursorPosX(windowWidth - 60); // 60 pixels from right edge
            ImGui::PushID(i);
            if (ImGui::Button(ICON_TRASH_CAN)) {
                common->favorite_jobs.removeJob(job_id);
				ImGui::PopID();
                i++;
                continue;
            }
			ImGui::PopID();
   
            ImGui::SetCursorPos(lineStart);

            // Job Title as header
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(66, 135, 245, 255));
            ImGui::TextWrapped("%s", job.title.c_str());
            ImGui::PopStyleColor();

            // Company
            ImGui::TextWrapped("Company: %s", job.company.c_str());

            // Location
            ImGui::TextWrapped("Location: %s", job.location.c_str());

            // Salary
            ImGui::TextWrapped("Salary: %s", job.salary.c_str());
            
            ImGui::TextWrapped("Description: %s", job.description.c_str());

            // Add some spacing between jobs
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            i++;
        }
        if (ImGui::Button("Save Favorites")) {
            common->favorite_jobs.saveFavorites();
			favorite_jobs_is_open = false;
        }
        ImGui::EndChild();
        ImGui::EndPopup();
    }

    // Pop all style modifications
    ImGui::PopStyleVar(2);
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
    ImGui::SetNextWindowSize(windowSize);

	ImGui::OpenPopup("Job Finder");
    if (ImGui::BeginPopupModal("Job Finder", &show_jobs_list)){
        if (ImGui::BeginTabBar("JobsTabBar"))
        {
            if (ImGui::BeginTabItem("All Jobs"))
            {
                display_job_table(common);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Statistics")) {
                if (common->stats_data_ready)
                    display_last_year_stats(*common);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Companies")) {
                if (common->companies_data_ready)
                    DrawPieChart(*common);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndPopup();
    }
}

void DrawThread::display_job_table(CommonObjects* common) {

    static int selected_job = -1;  // Track which job was clicked
    static bool show_job_details = false;       // Flag to open popup

    if (ImGui::BeginTable("JobTable", 7, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
    {
        // Table headers
        ImGui::TableSetupColumn("num", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Company", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Salary", ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthFixed, 30.0f);
        ImGui::TableSetupColumn("More", ImGuiTableColumnFlags_WidthFixed, 50.0f);

        ImGui::TableHeadersRow();

        // Iterate over the jobs and create rows in the table
        for (size_t i = 0; i < current_jobs.size(); ++i)
        {
            Job& job = current_jobs[i];
            ImGui::TableNextRow();
            ImGui::TableNextColumn();  // Move to first column
			ImGui::Text("%d", i + 1);
            ImGui::TableNextColumn();
            ImGui::TextWrapped("%s", job.title.c_str());
            ImGui::TableNextColumn();
            ImGui::TextWrapped("%s", job.company.c_str());
            ImGui::TableNextColumn();
            ImGui::TextWrapped("%s", job.location.c_str());
            ImGui::TableNextColumn();
            ImGui::TextWrapped("%s", job.salary.c_str());

            ImGui::TableNextColumn();
            std::string star_id = "star_button_" + std::to_string(i + 1);
            bool is_clicked = DrawStar(star_id.c_str(), job.is_starred);
			if (is_clicked) {
				if (job.is_starred) {
					common->favorite_jobs.addJob(job);
				}
				else {
					common->favorite_jobs.removeJob(job.id);
				}
			}
            ImGui::TableNextColumn();
            std::string button_id = "show##" + std::to_string(i);
			if (ImGui::Button(button_id.c_str(), ImVec2(50, 20))) {
				selected_job = i;
				show_job_details = true;
			}
        }
        ImGui::EndTable();
    }

    float button_width = 120.0f; // Width of the button

    // Handle popup
    if (show_job_details)
    {
        ImGui::OpenPopup("Job Details");

        // Center popup in the middle of the window
        /*ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));*/

        // Set fixed size for popup
        //ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Always);
        if (ImGui::BeginPopupModal("Job Details", &show_job_details))
        {

            // Begin a child window that will contain all content with vertical scrolling
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() - 8), true);
            if (selected_job >= 0 && selected_job < current_jobs.size())
            {
                Job& job = current_jobs[selected_job];

                ImGui::Text("Title: %s", job.title.c_str());
                ImGui::Separator();

                ImGui::Text("Company: %s", job.company.c_str());
                ImGui::Text("Location: %s", job.location.c_str());
                ImGui::Text("Salary: %s", job.salary.c_str());
                ImGui::Text("Posted on: %s", job.created_date.c_str());

                ImGui::Separator();
                ImGui::Text("Description:");
                ImGui::TextWrapped("%s", job.description.c_str());

                ImGui::Separator();
                ImGui::TextWrapped("URL: %s", job.url.c_str());

            }
            ImGui::EndChild();

            Job& job = current_jobs[selected_job];
            bool toRemove = common->favorite_jobs.isJobInFavorites(job.id);

            if (toRemove && jobsButton("Remove from favorite", button_width + 40)) {
                job.is_starred = false;
                common->favorite_jobs.removeJob(job.id);
                //ImGui::CloseCurrentPopup();
            }
            if (!toRemove && jobsButton("Add to favorite", button_width)) {
                job.is_starred = true;
                common->favorite_jobs.addJob(job);
                //ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();

        }
    }

    float window_width = ImGui::GetWindowSize().x;
    float button_x = (window_width - button_width) * 0.5f; // Center the button horizontally
    ImGui::SetCursorPosX(button_x);

    // Add button to load more jobs
    if (jobsButton("More Jobs", button_width))
    {
        common->current_page++;
        common->start_job_searching = true;
        common->cv.notify_one();
    }
    
}

bool DrawThread::DrawStar(const char* id, bool& is_starred)
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
    const int num_points = 5;
    ImVec2 points[num_points * 2];
    float start_angle = -IM_PI / 2; // Start with the top point of the star
    float angle_increment = IM_PI / num_points;
	float radius = size * 0.4f;

    for (int i = 0; i < num_points * 2; i++) {
        // Use consistent radii for all stars
        float r = (i % 2 == 0) ? radius : radius * 0.45f;
        float angle = start_angle + i * angle_increment;
        points[i] = ImVec2(center.x + r * cosf(angle), center.y + r * sinf(angle));
    }

    // Fill the star only if `filled` is true
    if (is_starred) {
        draw_list->AddConvexPolyFilled(points, num_points * 2, color); // Fill inside the star
    }

    // Draw the outer border on top to ensure it stays visible
    draw_list->AddPolyline(points, num_points * 2, IM_COL32(255, 255, 255, 255), true, 2.0f);

    ImGui::PopID();
	return clicked;
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
    ImPlot::CreateContext();
    float months[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    std::string title = "Average Salary Over 2024 in field: " + common.field;
    if (ImPlot::BeginPlot(title.c_str())) {
        ImPlot::SetupAxes("Month", "Average Salary ($)");
        ImPlot::PlotLine("Salary", months, common.salaries, 12);
        ImPlot::EndPlot();
    }
}


void DrawThread::DrawPieChart(CommonObjects& common) {
    ImPlot::CreateContext();
    const int size = common.company_names.size();
    std::vector<const char*> labels_vec;
    std::vector<float> percentages;
    float total = 0.0f;
    for (float value : common.company_values) {
        total += value;
    }
    for (int i = 0; i < size; ++i) {
        labels_vec.push_back(common.company_names[i].c_str());
        percentages.push_back((common.company_values[i] / total) * 100.0f);
    }
    static const ImVec4 colors[] = {
        ImVec4(0.9f, 0.1f, 0.1f, 1.0f),
        ImVec4(0.1f, 0.9f, 0.1f, 1.0f),
        ImVec4(0.1f, 0.1f, 0.9f, 1.0f),
        ImVec4(0.9f, 0.9f, 0.1f, 1.0f),
        ImVec4(0.9f, 0.1f, 0.9f, 1.0f)
    };

    std::string title = "Company Jobs Distribution in: " + common.country;
    ImGui::Text(title.c_str());

    ImGui::BeginGroup();

    ImPlotFlags plotFlags = ImPlotFlags_NoLegend;
    if (ImPlot::BeginPlot("Jobs Pie Chart", ImVec2(400, 400), plotFlags)) {
        ImPlot::PlotPieChart(labels_vec.data(), percentages.data(), size, 0.5f, 0.5f, 0.4f, "%.1f%%", 90.0f, 0);
        ImPlot::EndPlot();
    }
    ImGui::EndGroup();

    ImGui::SameLine(450); 

    ImGui::BeginGroup();
    ImGui::Text("Legend:");
    ImGui::Dummy(ImVec2(0, 10));

    for (int i = 0; i < size; ++i) {
        ImGui::ColorButton(labels_vec[i], colors[i % 5], ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20));
        ImGui::SameLine();
        ImGui::Text("%s: %.0f jobs (%.1f%%)", labels_vec[i], common.company_values[i], percentages[i]);
        ImGui::Dummy(ImVec2(0, 5)); 
    }
    ImGui::EndGroup();

    ImPlot::DestroyContext();
}





    

