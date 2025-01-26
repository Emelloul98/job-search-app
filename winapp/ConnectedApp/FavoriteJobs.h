#pragma once
/**
 * @file FavoriteJobs.h
 * @brief Header file for FavoriteJobs class
 */
#ifndef FAVORITE_JOBS_H
#define FAVORITE_JOBS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include "Job.h"
#include <unordered_map>


 /**
  * @class FavoriteJobs
  * @brief Manages a collection of favorite job listings with file persistence
  *
  * This class provides functionality to:
  * - Add new jobs to favorites
  * - Remove jobs from favorites
  * - Check if a job exists in favorites
  * - Get all favorite jobs
  * - Automatically save and load favorites from file
  */
class FavoriteJobs {
private:
    std::string filePath;
    std::unordered_map<std::string, Job> favorites;
    // loads jobs from file
    void loadFavorites();  

public:
    explicit FavoriteJobs(const std::string& path);
    bool addJob(const Job& job);
    bool removeJob(const std::string& jobId);
    std::unordered_map<std::string, Job> getFavorites();
    bool isJobInFavorites(const std::string& jobId) const;
    void saveFavorites();
};

#endif // FAVORITE_JOBS_H