#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

std::string app_id = "40ab1a9c";
std::string app_key = "cf73de75ea3954c26f2dabdaa9f233ef";
// Initialize the client with the base URL
httplib::Client cli("https://api.adzuna.com");
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Recipe, name, cuisine, difficulty, cookTimeMinutes ,image)
// Define how to parse the Jobs struct from the JSON response

void DownloadThread::operator()(CommonObjects& common)
{
    while (true) {
        {
            std::unique_lock<std::mutex> lock(common.mtx);
            common.cv.wait(lock, [&common]() {
                return common.start_job_searching.load();
            });
			if (common.exit_flag) return;
            searchJobs(common);
            common.start_job_searching=false;
			common.job_page_ready = true;
            std::cout << "Full data ready" << std::endl;
        }
    }
}

void DownloadThread::searchJobs(CommonObjects& common)
{
    std::string current_country = common.country;
    std::string current_category = common.field + "-jobs";
    // Construct the URL for the "teaching-jobs" category
    std::string url = "https://api.adzuna.com/v1/api/jobs/" + current_country + "/search/" + std::to_string(common.current_page) + "?app_id=" + app_id + "&app_key=" + app_key + "&results_per_page=10&category=" + current_category;

    if (!common.sorted_by.empty()) {
        common.sorted_by[0] = std::tolower(common.sorted_by[0]);
        url += "&sort_by=" + common.sorted_by;
    }
    if (common.job_type == "Full Time") {
        url += "&full_time=1";
    }
    else if (common.job_type == "Part Time") {
        url += "&part_time=1";
    }
	//std::cout << url << std::endl;
    // Send the GET request using the constructed URL
    auto res = cli.Get(url);

    if (res && (res->status >= 200 && res->status < 300)) {
        try {
            // Parse the raw JSON response
            nlohmann::json response = nlohmann::json::parse(res->body);
            common.jobs.clear();
            // Initialize the jobs vector from the response            
            for (const auto& job_data : response["results"]) {
                Jobs job;

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
                    auto area = job_data["location"]["area"];
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

std::string DownloadThread::sanitizeDescription(const std::string& desc) {
    std::string sanitized_desc = desc;
    // Remove non-ASCII characters or replace with a placeholder
    sanitized_desc.erase(std::remove_if(sanitized_desc.begin(), sanitized_desc.end(), [](char c) {
        return static_cast<unsigned char>(c) > 127;  // Non-ASCII characters
        }), sanitized_desc.end());

    return sanitized_desc;
}
std::string DownloadThread::formatNumberWithCommas(int number) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << number;
    return ss.str();
}








