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
    /*std::unique_lock<std::mutex> lock(common.mtx);
    common.cv.wait(lock, [&common]() {
       return common.start_download.load() || common.exit_flag.load();
       });
    if (common.exit_flag.load()) return;*/
    getCountryLabels(common);
	searchJobs(common);
}
void DownloadThread::getCountryLabels(CommonObjects& common)
{
    /*std::string current_country = common.country;*/
    std::string current_country = "us";// Todo: remove
    // Construct the URL using the variables
    std::string url = "https://api.adzuna.com/v1/api/jobs/" + current_country + "/categories?app_id=" + app_id + "&app_key=" + app_key;
    // Send the GET request using the constructed URL
    auto res = cli.Get(url);
    // Check the response
    if (res && res->status == 200) {
        try {
            nlohmann::json response = nlohmann::json::parse(res->body);
            common.labels.clear();
            for (const auto& category : response["results"]) {
                std::string label = category["label"];
                if (label != "Unknown") {
                    common.labels.push_back(label);
                }
            }
            common.data_ready.store(true);
            common.start_download.store(false);
            std::cout << "Data ready" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
    }
    else {
        std::cerr << "Error fetching job data: " << res.error() << std::endl;
    }
}
void DownloadThread::searchJobs(CommonObjects& common)
{
    std::string current_country = "us"; // Todo: remove, dynamic assignment later
    std::string current_category = "teaching-jobs"; // Todo: remove, dynamic assignment later

    // Construct the URL for the "teaching-jobs" category
    std::string url = "https://api.adzuna.com/v1/api/jobs/" + current_country + "/search/100?app_id=" + app_id + "&app_key=" + app_key + "&results_per_page=10&category=" + current_category;

    // Send the GET request using the constructed URL
    auto res = cli.Get(url);
    if (res && res->status == 200) {
        try {
            //std::cout << "Full Response: " << res->body << std::endl;
            // Parse the raw JSON response
            nlohmann::json response = nlohmann::json::parse(res->body);
            // Initialize the jobs vector from the response
            common.jobs.clear(); // Clear existing jobs data
            for (const auto& job_data : response["results"]) {
                Jobs job;
                // Extract title and company name
                job.title = job_data["title"];
                job.company = job_data["company"]["display_name"];
                // Extract location (take everything except the state)
                auto area = job_data["location"]["area"];
                std::string location_str = "";
                for (size_t i = 0; i < area.size() - 1; ++i) {
                    location_str += area[i].get<std::string>() + ", ";
                }
                if (!location_str.empty()) {
                    location_str = location_str.substr(0, location_str.size() - 2);  // Remove trailing comma
                }
                job.location = location_str;
                // Extract salary (only salary_min)
                job.salary = "$" + std::to_string(job_data["salary_min"].get<double>());
                // Extract job URL
                job.url = job_data["redirect_url"];
                // Extract job description and check its length
                job.description = sanitizeDescription(job_data["description"]);
                // Extract job created date
                job.created_date = job_data["created"];

                // Add the job to the jobs vector
                common.jobs.push_back(job);
            }

            /*std::cout << "Printing first 2-3 job objects from the vector:" << std::endl;
            for (size_t i = 0; i < std::min(common.jobs.size(), size_t(10)); ++i) {
                const Jobs& job = common.jobs[i];
                std::cout << "Job " << i + 1 << ": " << std::endl;
                std::cout << "Title: " << job.title << std::endl;
                std::cout << "Company: " << job.company << std::endl;
                std::cout << "Location: " << job.location << std::endl;
                std::cout << "Salary: " << job.salary << std::endl;
                std::cout << "URL: " << job.url << std::endl;
                std::cout << "Description: " << job.description << std::endl;
                std::cout << "Created Date: " << job.created_date << std::endl;
                std::cout << "-------------------------------" << std::endl;
            }*/
            // Indicate that the job data is ready
            common.data_ready.store(true);
            common.start_download.store(false);
            std::cout << "Job data initialized." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON in searchJobs: " << e.what() << std::endl;
        }
    }
    else {
        std::cerr << "Error fetching job data: " << res.error() << std::endl;
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









