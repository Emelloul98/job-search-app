#pragma once
#include "CommonObject.h"
#include <iostream>
class DownloadThread
{
public:
	void operator()(CommonObjects& common);
	void getCountryLabels(CommonObjects& common);
	void searchJobs(CommonObjects& common);
	std::string sanitizeDescription(const std::string& description);
	std::string formatNumberWithCommas(int number);
private:
	std::string _download_url;
};

