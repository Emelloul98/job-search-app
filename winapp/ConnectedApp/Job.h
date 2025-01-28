#pragma once
#include <string>

struct Job
{
	std::string title;          // Job title (e.g., "Substitute Teacher K-12, get paid weekly")
	std::string company;        // Company name (e.g., "Swing Education")
	std::string location;       // (e.g., "Clifton, Passaic County")
	std::string url;            // URL for job details 
	std::string salary;         // Salary (e.g., "36940.19")
	std::string description;    // Job description (e.g., "Step in for absent teachers with Swing Education...")
	std::string created_date;   // Date the job was posted (e.g., "2025-01-02T15:09:11Z")
	std::string id;             // Unique job ID
	bool is_expanded = false;   // Flag to indicate if the job details are expanded
	bool is_starred= false;    // Flag to indicate if the job is starred
};