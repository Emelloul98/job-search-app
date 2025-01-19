#pragma once
#include "CommonObject.h"
#include <d3d11.h>
#include "../../shared/ImGuiSrc/imgui.h"
#include "implot.h"

class DrawThread
{
public:
    // Main functions:
    void operator()(CommonObjects& common);
    void RenderSearchBar(CommonObjects* common);
    void RenderBackgroundImage(CommonObjects* common);
    void RenderCustomComboBox(const char* label, const char* items[], size_t items_count, int* selected_item, float column_width);
    void display_jobs(CommonObjects* common);

    // Side functions:
    ID3D11ShaderResourceView* CreateTextureFromImage(const unsigned char* image_data, int width, int height, int channels);
    void DrawStar(ImDrawList* draw_list, ImVec2 center, float radius, ImU32 color, bool filled); 
    bool StarButton(const char* id, bool& is_starred);
    bool jobsButton(const char* label, float button_width);
    void display_last_year_stats(CommonObjects& common);
    void saveStarredJobsToFile();

	// Variables:
    std::vector<Jobs> current_jobs;
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
    bool show_jobs_list = false;
    bool show_last_year_stats = false;
    bool starred_file_exists = false;
};
void DrawAppWindow(void* common_ptr, void* callerPtr);
