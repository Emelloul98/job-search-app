#pragma once
#include <atomic>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include "FavoriteJobs.h"


struct CommonObjects
{
	std::atomic_bool exit_flag = false;
	std::atomic_int current_page = 1;
	std::atomic_bool start_job_searching = false;
	std::atomic_bool job_page_ready = false;
	std::atomic_bool stats_data_ready = false;
	std::atomic_bool companies_data_ready = false;
	std::atomic_bool save_favorites_to_file = false;
	std::atomic_bool show_more_jobs_button = false;
	std::atomic_bool no_jobs_at_all = false;


	std::string url;
	std::vector<Job> jobs;
	std::mutex mtx;
	std::condition_variable cv;
	std::string country;
	std::string field;
	std::string job_type;
	std::string max_days_old;
	float salaries[12] = { 0 };
	FavoriteJobs favorite_jobs = FavoriteJobs("favorite_jobs.json");
	std::vector<std::string> company_names; 
	std::vector<float> company_values;

};
