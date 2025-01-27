#pragma once
#include "DrawThread.h"
#include "GuiMain.h"
#include <iostream>
#include <d3d11.h>
#include "stb_image.h"
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
constexpr double IM_PI = 3.14159265358979323846f;

void DrawThread:: operator()(CommonObjects& common) {
    // Create a new ImPlot context:
    ImPlot::CreateContext();

    GuiMain(DrawAppWindow, &common, this);
    common.exit_flag = false;
    common.save_favorites_to_file = true;
    common.cv.notify_one();
    common.exit_flag = true;
	common.cv.notify_one();

}

void DrawAppWindow(void* common_ptr,void* callerPtr) {
    auto common = static_cast<CommonObjects*>(common_ptr);
    auto draw_thread = (DrawThread*)callerPtr;
	// image currently not working:
	if (draw_thread->texture == nullptr) draw_thread->InitializeTextures();
	// Render the background image:
    draw_thread->RenderBackgroundImage();
	// Render the search bar:
    draw_thread->RenderSearchBar(*common);
    if (common->job_page_ready) 
    {
		// Add the new jobs to the jobs vector:
        draw_thread->current_jobs.insert(draw_thread->current_jobs.end(), common->jobs.begin(), common->jobs.end());
		common->job_page_ready = false;
		draw_thread->show_jobs_list = true;
    }
	// Display the frame pages:
    if(draw_thread->show_jobs_list)
        draw_thread->display_frame_pages(*common);
    // Close the application:
    if (common->exit_flag)
    {
		common->cv.notify_one();
        return;
    }
}

void DrawThread::InitializeTextures() {
    int image_width = 0, image_height = 0;
	// Load the background image with  GuiMain loadTextureFromFile function:
    bool success = LoadTextureFromFile("../../images/background.jpg", &texture, &image_width, &image_height);
    if (!success) {
        std::cerr << "Failed to load image!" << std::endl;
    }
}

void DrawThread::RenderBackgroundImage() const {
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

void DrawThread:: RenderSearchBar(CommonObjects& common) {
    ImVec2 window_size = ImGui::GetIO().DisplaySize;
    float searchbar_width = window_size.x * 0.8f;  
    float center_position = (window_size.x - searchbar_width) * 0.5f;
    float searchbar_y_pos = window_size.y * 0.4f; 
	float searchbar_height = window_size.y * 0.12f;  

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
    //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 8));


    // Center the SearchBar
    
    ImGui::SetCursorPosX(center_position);
    ImGui::SetCursorPosY(searchbar_y_pos); 


    // Begin main container
    ImGui::BeginChild("SearchBar", ImVec2(searchbar_width, searchbar_height), true,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);

    ImGui::SetCursorPos(ImVec2(0, 0));

    ImGui::BeginGroup();
    ImGui::Columns(5, nullptr, false);

    float column_width = window_size.x*0.185f;
    float button_column_width = window_size.x*0.06f;  // Width for the button column

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
    float search_icon_size = window_size.y*0.06f;
    float search_icon_pos = (searchbar_height - search_icon_size) * 0.5f;

    ImGui::PopStyleVar(2);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, search_icon_size / 2.0f); 

    ImGui::SetCursorPosY(search_icon_pos); // Only adjust Y position
	// Search button:
    if (ImGui::Button(ICON_MAGNIFYING_GLASS, ImVec2(search_icon_size, search_icon_size))) {

        if (selected_job_type != -1 && selected_days_old != -1 && selected_field != -1 && selected_location != -1) {
			// Defualt values:
            common.job_page_ready = false;
            common.stats_data_ready = false;
            common.companies_data_ready = false;
			common.current_page = 1;
			common.exit_flag = false;
			common.show_more_jobs_button = false;
			common.no_jobs_at_all = false;
			// Set the search parameters:
            common.country = country_codes.at(locations[selected_location]);
			common.field = fields[selected_field];
			common.job_type = job_types[selected_job_type];
            common.max_days_old = max_days_old[selected_days_old];
			current_jobs.clear();
            common.start_job_searching=true;
            common.cv.notify_one();  
          
        }
    }
   
    ImGui::EndGroup();
    ImGui::EndChild();

    // Add Favorites button below the search bar
    const float favorites_button_width = window_size.x*0.15f;  // Width of favorites button
    const float favorites_button_height = window_size.y * 0.08f;  // Height of favorites button

     // Calculate position for centered favorites button
    float favorites_x = (window_size.x - favorites_button_width) * 0.5f;
    float favorites_y = window_size.y * 0.85f;

    static bool favorite_jobs_is_open = false;

    // Style for favorites button
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    
    ImGui::SetCursorPos(ImVec2(favorites_x, favorites_y));

    if (ImGui::Button("Favorites "  ICON_FA_HEART, ImVec2(favorites_button_width, favorites_button_height))) {
		favorite_jobs_is_open = true;
        ImGui::OpenPopup("Favorite Jobs");
    }
    // Popup window

    ImVec2 favorites_window_size = ImVec2(window_size.x*0.5f, window_size.y*0.6f); 
    ImVec2 window_pos = ImVec2(
        (window_size.x - favorites_window_size.x) * 0.5f,
        (window_size.y - favorites_window_size.y) * 0.5f
    );

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing);
    // Set fixed size for popup
    ImGui::SetNextWindowSize(favorites_window_size);

    if (ImGui::BeginPopupModal("Favorite Jobs", &favorite_jobs_is_open)) {
        std::unordered_map<std::string, Job> favoriteJobs  = common.favorite_jobs.getFavorites();

		ImGui::BeginChild("ScrollingRegion", ImVec2(window_size.x * 0.485f, window_size.y * 0.5f), true); 

        for (const auto& [job_id, job] : favoriteJobs) {
            ImVec2 lineStart = ImGui::GetCursorPos();

            float windowWidth = ImGui::GetWindowWidth();
            ImGui::SetCursorPosX(windowWidth - window_size.x*0.037f); 
            ImGui::PushID(job_id.c_str());
            if (ImGui::Button(ICON_TRASH_CAN)) {
                common.favorite_jobs.removeJob(job_id);
				ImGui::PopID();
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
        }
        ImGui::EndChild();

       
		std::string title = "Save Favorites";
        float center_x = ImGui::GetWindowWidth() / 2;
		// Center the button:
		float button_width = ImGui::CalcTextSize(title.c_str()).x + window_size.x*0.005f; 
		float buttom_height = window_size.y * 0.036f;

        ImVec2 button_size(button_width, buttom_height);

        ImGui::SetCursorPosX(center_x - button_width / 2);

		// Save favorites button:
        if (ImGui::Button(title.c_str(), button_size)) {
			common.save_favorites_to_file = true;
			common.cv.notify_one();
			favorite_jobs_is_open = false;
        }
        ImGui::EndPopup();
    }

    // Pop all style modifications
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(6);
    ImGui::End();

}

void DrawThread::RenderCustomComboBox(const char* label, const char* items[], size_t items_count, int* selected_item, float column_width) {

    ImVec2 window_size = ImGui::GetIO().DisplaySize;

    ImVec2 screenPos = ImGui::GetCursorScreenPos();

    // Get the current cursor position relative to the window
    ImVec2 pos = ImGui::GetCursorPos();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImGui::PushID(label);
    float searchbar_height = window_size.y*0.12f;

    // Calculate label position
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float labelOffsetX = (column_width - labelSize.x) * 0.5f;

    const char* preview = (*selected_item >= 0) ? items[*selected_item] : "Choose...";
    ImVec2 previewSize = ImGui::CalcTextSize(preview);
    float previewOffsetX = (column_width - previewSize.x) * 0.5f;
    float space_between_text = window_size.y * 0.004f;
    float text_height = previewSize.y + labelSize.y + space_between_text;
	float text_offset_y = (searchbar_height - text_height)*0.5f;
    ImGui::BeginGroup();

    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.85f, 0.85f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.75f, 0.75f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);

	float combo_x_offset = window_size.x * 0.007f;
    float combo_y_offset = window_size.y * 0.013f;
	float combo_height = searchbar_height - (combo_y_offset * 2);
	float combo_width = column_width - (combo_x_offset * 2);
    ImGui::SetCursorPos(ImVec2(pos.x + combo_x_offset, combo_y_offset));

	bool combo_clicked = ImGui::Button("##combo", ImVec2(combo_width, combo_height));

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    // Set relative position for the label
    ImGui::SetCursorPos(ImVec2(pos.x + labelOffsetX, pos.y + text_offset_y));

    // Add some contrast to make the text visible
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(pos.x + previewOffsetX, pos.y + text_offset_y + labelSize.y + space_between_text));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::Text("%s", preview);
    ImGui::PopStyleColor();
	// Draw a line below the combo box:
	float line_height = searchbar_height*0.7f;
	float line_y_offset = (searchbar_height - line_height) * 0.5f;
    if (label != "Location") {
        ImGui::SameLine();
        draw_list->AddLine(
            ImVec2(screenPos.x, screenPos.y + line_y_offset),
            ImVec2(screenPos.x, screenPos.y + line_y_offset + line_height),
            IM_COL32(211, 211, 211, 255),
            1.0f
        );
    }
	// Open the popup when the button is clicked:
    if (combo_clicked) ImGui::OpenPopup(label);
    ImGui::SetNextWindowPos(ImVec2(screenPos.x + combo_x_offset, screenPos.y + searchbar_height ));

    // Calculate the height needed for all items
    float item_height = ImGui::GetTextLineHeightWithSpacing();
    float content_height = item_height * items_count;
    float popup_height = content_height + ImGui::GetStyle().WindowPadding.y * 2;
    if (popup_height > window_size.y*0.3) popup_height = window_size.y * 0.3f;

    ImGui::SetNextWindowSize(ImVec2(combo_width, popup_height));
    
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

void DrawThread::display_frame_pages(CommonObjects& common)
{
    ImVec2 window_size = ImGui::GetIO().DisplaySize;
	// Popup window size:
    ImVec2 job_finder_window_size = ImVec2(window_size.x*0.8f, window_size.y*0.74f);
	// Center the popup window:
    ImVec2 window_pos = ImVec2(
        (window_size.x - job_finder_window_size.x) * 0.5f,
        (window_size.y - job_finder_window_size.y) * 0.5f
    );

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing);
    // Set fixed size for popup
    ImGui::SetNextWindowSize(job_finder_window_size);

    static bool first_time = true;
	// Open the popup with 3 tabs:
	ImGui::OpenPopup("Job Finder");
    if (ImGui::BeginPopupModal("Job Finder", &show_jobs_list)){

        if (ImGui::BeginTabBar("JobsTabBar", ImGuiTabBarFlags_AutoSelectNewTabs))
        {
			// Display the All Jobs tab:
            if (ImGui::BeginTabItem("All Jobs", nullptr, first_time ? ImGuiTabItemFlags_SetSelected : 0)) {
				if (common.no_jobs_at_all) {
					ImGui::Text("No jobs found for the selected criteria.");
				}
                else {
                    display_job_table(common);
                }
                ImGui::EndTabItem();
                first_time = false;
            }
			// Display the Favorites tab:
            if (ImGui::BeginTabItem("Statistics")) {
                if (common.stats_data_ready)
                    display_last_year_stats(common);
                ImGui::EndTabItem();
            }
			// Display the Companies tab:
            if (ImGui::BeginTabItem("Companies")) {
                if (common.companies_data_ready)
                    DrawPieChart(common);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::EndPopup();
    }
	// Reset the first_time flag:
    if (!show_jobs_list) {
        first_time = true;
    }

}

void DrawThread::display_job_table(CommonObjects& common) {

    static int selected_job = -1;  // Track which job was clicked
    static bool show_job_details = false;       // Flag to open popup
    ImVec2 window_size = ImGui::GetIO().DisplaySize;


    float table_height = window_size.y*0.59f;
    ImGui::BeginChild("TableScrollingRegion", ImVec2(0, table_height), true);
	// Jobs Table initialization:
    if (ImGui::BeginTable("JobTable", 7, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp))
    {
        // Table headers:
        ImGui::TableSetupColumn("num", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Company", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Salary", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Star", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("View", ImGuiTableColumnFlags_WidthStretch);
        
        ImGui::TableHeadersRow();

		float column_width = 0.0f;
		float text_width = 0.0f;
        // Iterate over the jobs and create rows in the table
        for (size_t i = 0; i < current_jobs.size(); ++i)
        {
            Job& job = current_jobs[i];
            ImGui::TableNextRow();
            ImGui::TableNextColumn();  
            ImGui::AlignTextToFramePadding();
			ImGui::Text("%d", i + 1);
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped("%s", job.title.c_str());
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped("%s", job.company.c_str());
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped("%s", job.location.c_str());
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::TextWrapped("%s", job.salary.c_str());

            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
			// Create a unique ID for each star button:
            std::string star_id = "star_button_" + std::to_string(i + 1);
            bool is_clicked = DrawStar(star_id.c_str(), job.is_starred);
			// Add the job to favorites if the star is clicked
			if (is_clicked) {
				// Add the job to favorites if the star is clicked:
				if (job.is_starred) {
					common.favorite_jobs.addJob(job);
				}
				// Remove the job from favorites if the star is clicked again:
				else {
					common.favorite_jobs.removeJob(job.id);
				}
			}
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            column_width = ImGui::GetColumnWidth();
            text_width = ImGui::CalcTextSize(ICON_FA_EYE).x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
            ImGui::Text(ICON_FA_EYE);
			// Open the job details popup when the view button is clicked:
            if (ImGui::IsItemClicked()) {
                selected_job = static_cast<int>(i);
                show_job_details = true;
            }
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();

	float button_width = window_size.x*0.1f; 

    // Handle popup
    if (show_job_details)
    {
        ImGui::OpenPopup("Job Details");

        // Center popup in the middle of the window
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        // Set fixed size for popup
        ImGui::SetNextWindowSize(ImVec2(window_size.x*0.5f, window_size.y*0.69f), ImGuiCond_Always);
        if (ImGui::BeginPopupModal("Job Details", &show_job_details))
        {

            // Begin a child window that will contain all content with vertical scrolling
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, window_size.y * 0.58f), true);
            // to remove:
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
            bool toRemove = common.favorite_jobs.isJobInFavorites(job.id);

            float window_width = ImGui::GetWindowWidth();
            float remove_x_pos = (window_width - (button_width*1.5f) ) / 2;
			float add_x_pos = (window_width - button_width) / 2;
            ImGui::SetCursorPosX(remove_x_pos);
            
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
			if (toRemove && jobsButton("Remove from favorite", button_width*1.5f, "red")) { 
                job.is_starred = false;
                common.favorite_jobs.removeJob(job.id);
            }
            ImGui::SetCursorPosX(add_x_pos);

            if (!toRemove && jobsButton("Add to favorite", button_width,"orange")) {
                job.is_starred = true;
                common.favorite_jobs.addJob(job);
            }
            ImGui::PopStyleVar();

            ImGui::EndPopup();

        }
    }

    float window_width = ImGui::GetWindowSize().x;
    float button_x = (window_width - button_width) * 0.5f; // Center the button horizontally
    ImGui::SetCursorPosX(button_x);

    if (common.show_more_jobs_button) {
        // Add button to load more jobs
        if (jobsButton("More Jobs", button_width, "blue"))
        {
            common.current_page++;
            common.start_job_searching = true;
            common.cv.notify_one();
        }
    }
	ImGui::SameLine();
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
	// Toggle the state when clicked:
    if (clicked)
        is_starred = !is_starred; 

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

bool DrawThread::jobsButton(const char* label, float button_width, const std::string& color_scheme) {
    // Push an ID to ensure the button is unique:
    ImGui::PushID(label);
    ImVec2 button_size = ImVec2(button_width, 30);
    ImVec2 p = ImGui::GetCursorScreenPos();
    bool is_clicked = ImGui::InvisibleButton(label, button_size);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImU32 color_bg, color_hover, color_text;
    // Default colors for the "purple" scheme:
    if (color_scheme == "blue") {
        /*color_bg = ImGui::GetColorU32(ImVec4(0.15f, 0.16f, 0.27f, 1.0f));*/
        color_bg = ImGui::GetColorU32(ImVec4(0.18f, 0.23f, 0.65f, 1.0f));
        color_hover = ImGui::GetColorU32(ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        color_text = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    else if (color_scheme == "orange") {
        color_bg = ImGui::GetColorU32(ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
        color_hover = ImGui::GetColorU32(ImVec4(1.0f, 0.6f, 0.1f, 1.0f));
        color_text = ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    }
    else // Red:
    {
        color_bg = ImGui::GetColorU32(ImVec4(0.85f, 0.15f, 0.15f, 1.0f));
        color_hover = ImGui::GetColorU32(ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        color_text = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // Draw the button with rounded corners:
    float radius = 10.0f; // Adjust the radius for rounded corners
    draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + button_size.x, p.y + button_size.y), color_bg, radius);
    if (ImGui::IsItemHovered())
        draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + button_size.x, p.y + button_size.y), color_hover, radius);

    // Center the text
    ImVec2 text_size = ImGui::CalcTextSize(label);
    ImVec2 text_pos = ImVec2(p.x + (button_size.x - text_size.x) * 0.5f, p.y + (button_size.y - text_size.y) * 0.5f);
    draw_list->AddText(text_pos, color_text, label);

    ImGui::PopID();

    // Return true if the button is clicked:
    return is_clicked;
}

void DrawThread::display_last_year_stats(CommonObjects& common) {
	
    float months[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    std::string title = "Average Salary Over 2024 in field: " + common.field;
	// Plot the line chart:
    if (ImPlot::BeginPlot(title.c_str())) {
        ImPlot::SetupAxes("Month", "Average Salary ($)");
        ImPlot::PlotLine("Salary", months, common.salaries, 12);
        ImPlot::EndPlot();
    }
}


void DrawThread::DrawPieChart(CommonObjects& common) {
    const int size = static_cast<int>(common.company_names.size());
    std::vector<const char*> labels_vec;
    std::vector<float> percentages;
    float total = 0.0f;
	// Calculate the total number of jobs:
    for (float value : common.company_values) {
        total += value;
    }
	// Calculate the percentages:
    for (int i = 0; i < size; ++i) {
        labels_vec.push_back(common.company_names[i].c_str());
        percentages.push_back((common.company_values[i] / total) * 100.0f);
    }
	// Set the colors for the pie chart:
    static const ImVec4 colors[] = {
        ImVec4(0.9f, 0.1f, 0.1f, 1.0f),
        ImVec4(0.1f, 0.9f, 0.1f, 1.0f),
        ImVec4(0.1f, 0.1f, 0.9f, 1.0f),
        ImVec4(0.9f, 0.9f, 0.1f, 1.0f),
        ImVec4(0.9f, 0.1f, 0.9f, 1.0f)
    };
	// Set the title of the pie chart:
    std::string title = "Company Jobs Distribution in: " + common.country;
    ImGui::Text(title.c_str());

    ImGui::BeginGroup();
	// Plot the pie chart:
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
	// Display the color buttons and the company names:
    for (int i = 0; i < size; ++i) {
        ImGui::ColorButton(labels_vec[i], colors[i % 5], ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20));
        ImGui::SameLine();
        ImGui::Text("%s: %.0f jobs (%.1f%%)", labels_vec[i], common.company_values[i], percentages[i]);
        ImGui::Dummy(ImVec2(0, 5)); 
    }
    ImGui::EndGroup();

}





    

