#pragma once
#include <atomic>
#include <string>
#include <vector>
struct Jobs
{
	std::string title;
	std::string company;
	std::string location;
	std::string url;
	std::string description;
	std::string salary;
	std::string type;
};

struct CommonObjects
{
	std::atomic_bool exit_flag = false;
	std::atomic_bool start_download = false;
	std::atomic_bool data_ready = false;
	std::string url;
	std::vector<Jobs> jobs;
};


//struct Recipe
//{
//	std::string name;
//	std::string cuisine;
//	std::string difficulty;
//	int cookTimeMinutes;
//	std::string image;
//};