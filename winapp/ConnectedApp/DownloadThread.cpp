#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"
// API credentials:
std::string app_id = "40ab1a9c";
std::string app_key = "cf73de75ea3954c26f2dabdaa9f233ef";
// Initialize the client with the base URL
httplib::Client cli("https://api.adzuna.com");

void DownloadThread::operator()(CommonObjects& common)
{
    while (true) {
        {
			// Wait for the condition variable to be notified:
            std::unique_lock<std::mutex> lock(common.mtx);
            common.cv.wait(lock, [&common]() {
                return common.start_job_searching.load() || common.save_favorites_to_file.load() || common.exit_flag.load();
            });
            // Check if the save_favorites_to_file flag is set:
            if (common.save_favorites_to_file) {
                common.favorite_jobs.saveFavorites();
                common.save_favorites_to_file = false;
            }
			// Check if the exit flag is set:
            if (common.exit_flag) {
                return;
            }
			// Check if the start_job_searching flag is set:
            if (common.start_job_searching) 
            {
                searchJobs(common);
                common.start_job_searching = false;
                common.job_page_ready = true;
				// If it's the first page, download the stats and companies data:
                if (common.current_page == 1) 
                {
                    downloadLastYearStats(common);
                    common.stats_data_ready = true;
                    downloadCompaniesData(common);
                    common.companies_data_ready = true;
                }
            }
			
        }
    }
}

void DownloadThread::searchJobs(CommonObjects& common)
{
    std::string current_country = common.country;
    std::string current_category = common.field + "-jobs";
    // Construct the URL for the "teaching-jobs" category
    std::string url = "https://api.adzuna.com/v1/api/jobs/" + current_country + "/search/" + std::to_string(common.current_page) + "?app_id=" + app_id + "&app_key=" + app_key + "&results_per_page=10";
    if (common.max_days_old != "All") {
        url += "&max_days_old=" + common.max_days_old;
    }
    url += "&category=" + current_category;
	url += "&sort_by=date";
    if (common.job_type == "Full Time") {
        url += "&full_time=1";
    }
    else if (common.job_type == "Part Time") {
        url += "&part_time=1";
    }
    // Send the GET request using the constructed URL
    auto res = cli.Get(url);

    if (res && (res->status >= 200 && res->status < 300)) {
        try {
            // Parse the raw JSON response
            nlohmann::json response = nlohmann::json::parse(res->body);

            double total_pages = ceil(response["count"].get<int>() / 10);
			// Check if there are no jobs at all:
			if (total_pages == 0) {
                common.no_jobs_at_all = true;
			}
			// Check if there are more pages to load:
			if (common.current_page < total_pages) {
				common.show_more_jobs_button = true;
            }
			// Check if there are no more pages to load:
            else {
				common.show_more_jobs_button = false;
            }
                
            common.jobs.clear();
            // Initialize the jobs vector from the response            
            for (const auto& job_data : response["results"]) {
                Job job;
                // Extract title and company name (check if they exist)
                job.title = job_data.contains("title") ? job_data["title"].get<std::string>() : "Unknown Title";
                if (job_data.contains("company") && job_data["company"].contains("display_name")) {
                    job.company = job_data["company"]["display_name"].get<std::string>();
                }
                else {
                    job.company = "Unknown Company";
                }
                // Extract location (check if "location" and "area" exist)
                if (job_data.contains("location") && job_data["location"].contains("area")) {
                    //auto area = job_data["location"]["area"];
                    auto& area = job_data["location"]["area"];
                    std::string location_str = "";
                    for (size_t i = 0; i < area.size() - 1; ++i) {
                        location_str += area[i].get<std::string>() + ", ";
                    }
                    if (!location_str.empty()) {
                        // Remove trailing comma
                        location_str = location_str.substr(0, location_str.size() - 2);
                    }
                    job.location = location_str;
                }
                else {
                    job.location = "Unknown Location";
                }

                // Extract salary (check for salary_min and salary_max)
                if (job_data.contains("salary_min")) {
                    job.salary = "$" + formatNumberWithCommas(static_cast<int>(job_data["salary_min"].get<double>()));
                }
                else if (job_data.contains("salary_max")) {
                    job.salary = "$" + formatNumberWithCommas(static_cast<int>(job_data["salary_max"].get<double>()));
                }
                else {
                    job.salary = "Not Specified";
                }

                // Extract job URL
                job.url = job_data.contains("redirect_url") ? job_data["redirect_url"].get<std::string>() : "Unknown URL";

                // Extract job description (check if exists)
                job.description = job_data.contains("description") ? sanitizeDescription(job_data["description"].get<std::string>()) : "No Description";

                // Extract job created date (check if exists)
                job.created_date = job_data.contains("created") ? job_data["created"].get<std::string>() : "Unknown Date";

				// Extract job ID (check if exists)
                job.id = job_data.contains("id") ? job_data["id"].get<std::string>() : "Unknown ID";

				job.is_starred = common.favorite_jobs.isJobInFavorites(job.id);
                // Add the job to the jobs vector
                common.jobs.push_back(job);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON in searchJobs: " << e.what() << std::endl;
        }
    }
    else {
        std::cerr << "Error fetching job data: " << (res ? std::to_string(res->status) : "Request failed") << std::endl;
    }
}

void DownloadThread::downloadLastYearStats(CommonObjects& common) {
    // Construct the URL for the historical salary stats endpoint
    std::string url = "https://api.adzuna.com/v1/api/jobs/" + common.country + "/history?app_id=" + app_id + "&app_key=" + app_key + "&category=" + common.field + "-jobs";

    // Send the GET request using the constructed URL
    auto res = cli.Get(url);

    if (res && (res->status >= 200 && res->status < 300)) {
        try {
            // Parse the raw JSON response
            nlohmann::json response = nlohmann::json::parse(res->body);
			//std::cout << response << std::endl;
            // Extract the "month" object which contains the salary data for each month
            if (response.contains("month")) {
                const auto& month_data = response["month"];

                // Clear previous salaries data
                std::fill(std::begin(common.salaries), std::end(common.salaries), 0.0f);  // Reset all values to 0

                // Loop through the months (January to December) and add values to the salaries array
                for (int month = 1; month <= 12; month++) {
                    // Build the month string with the proper format (2024-01, 2024-02, ..., 2024-12)
					std::string addition = month < 10 ? "0" : "";
                    std::string month_str = "2024-" + addition + std::to_string(month);

                    if (month_data.contains(month_str)) {
                        // Store the value in the corresponding index of the array
                        common.salaries[month - 1] = month_data[month_str].get<float>();
                    }
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON in downloadLastYearStats: " << e.what() << std::endl;
        }
    }
    else {
        std::cerr << "Error fetching stats data: " << (res ? std::to_string(res->status) : "Request failed") << std::endl;
    }
}

void DownloadThread::downloadCompaniesData(CommonObjects& common) {
    // Construct the URL for the top companies endpoint
    std::string url = "https://api.adzuna.com/v1/api/jobs/" + common.country + "/top_companies?app_id=" + app_id + "&app_key=" + app_key;

    // Send the GET request using the constructed URL
    auto res = cli.Get(url);

    if (res && (res->status >= 200 && res->status < 300)) {
        try {
            // Parse the raw JSON response
            nlohmann::json response = nlohmann::json::parse(res->body);

            // Extract the leaderboard data
            if (response.contains("leaderboard")) {
                const auto& leaderboard = response["leaderboard"];

                // Clear previous company data
                common.company_names.clear();
                common.company_values.clear();

                // Populate company names and values
                for (const auto& company_data : leaderboard) {
                    if (company_data.contains("canonical_name") && company_data.contains("count")) {
                        common.company_names.push_back(company_data["canonical_name"].get<std::string>());
                        common.company_values.push_back(company_data["count"].get<float>());
                    }
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON in downloadCompaniesData: " << e.what() << std::endl;
        }
    }
    else {
        std::cerr << "Error fetching companies data: " << (res ? std::to_string(res->status) : "Request failed") << std::endl;
    }
}



std::string DownloadThread::sanitizeDescription(const std::string& desc) {
    std::string sanitized_desc = desc;
    // Remove non-ASCII characters or replace with a placeholder
    sanitized_desc.erase(std::remove_if(sanitized_desc.begin(), sanitized_desc.end(), [](char c) {
        return static_cast<unsigned char>(c) > 127;  // Non-ASCII characters
        }), sanitized_desc.end());

    return sanitized_desc;
}

// Format the number with commas (e.g., 1000000 -> 1,000,000):
std::string DownloadThread::formatNumberWithCommas(int number) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << number;
    return ss.str();
}








