#pragma once
#include "CommonObject.h"
#include <iostream>
class DownloadThread
{
public:
	void operator()(CommonObjects& common);
	void searchJobs(CommonObjects& common);
	void downloadLastYearStats(CommonObjects& common);
	std::string sanitizeDescription(const std::string& description);
	std::string formatNumberWithCommas(int number);
	void downloadCompaniesData(CommonObjects& common);

};

