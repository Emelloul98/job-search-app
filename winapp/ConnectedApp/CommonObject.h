#pragma once
#include <atomic>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

struct Jobs
{
	std::string title;          // Job title (e.g., "Substitute Teacher K-12, get paid weekly")
	std::string company;        // Company name (e.g., "Swing Education")
	std::string location;       // (e.g., "Clifton, Passaic County")
	std::string url;            // URL for job details 
	std::string salary;         // Salary (e.g., "36940.19")
	std::string description;    // Job description (e.g., "Step in for absent teachers with Swing Education...")
	std::string created_date;   // Date the job was posted (e.g., "2025-01-02T15:09:11Z")
	bool is_expanded = false;   // Flag to indicate if the job details are expanded
	bool is_starred = false;    // Flag to indicate if the job is starred
};

struct CommonObjects
{
	std::atomic_bool exit_flag = false;
	std::atomic_bool start_job_searching = false;
	std::atomic_bool job_page_ready = false;
	std::string url;
	std::vector<Jobs> jobs;
	std::mutex mtx;
	std::condition_variable cv;
	std::string country;
	std::string field;
	std::string job_type;
	std::string sorted_by;
	std::atomic_int current_page = 1;
};
