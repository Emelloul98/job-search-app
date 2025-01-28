#include "FavoriteJobs.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

FavoriteJobs::FavoriteJobs(const std::string& path) : filePath(path) {
    loadFavorites();
}

void FavoriteJobs::loadFavorites() {
	// Open file for reading:
    std::ifstream file(filePath);
    if (!file) {
        return;
    }
	// Parse JSON data from file:
    try {
        json j = json::parse(file);
        for (const auto& item : j.items()) {
            Job job;
            job.id = item.key();
            job.title = item.value()["title"];
            job.company = item.value()["company"];
            job.location = item.value()["location"];
            job.url = item.value()["url"];
            job.salary = item.value()["salary"];
            job.description = item.value()["description"];
            job.created_date = item.value()["created_date"];
            job.is_starred = item.value()["is_starred"];
            favorites[job.id] = job;
        }
    }
	// Handle exceptions:
    catch (const std::exception& e) {
        std::cerr << "Error loading favorites: " << e.what() << std::endl;
        favorites.clear();
    }
	// Catch all other exceptions:
    catch (...) {
        std::cerr << "Unknown error loading favorites" << std::endl;
        favorites.clear();
    }
}
// Add job to favorites:
bool FavoriteJobs::addJob(const Job& job) {
    if (favorites.find(job.id) != favorites.end()) {
        return false;
    }
    favorites[job.id] = job;
    return true;
}
// Remove job from favorites:
bool FavoriteJobs::removeJob(const std::string& jobId) {
    if (favorites.erase(jobId) == 0) {
        return false;
    }
    return true;
}
// Get all favorite jobs:
std::unordered_map<std::string, Job> FavoriteJobs::getFavorites() {
    return favorites;
}
// Check if job is in favorites:
bool FavoriteJobs::isJobInFavorites(const std::string& jobId) const {
    return favorites.find(jobId) != favorites.end();
}
// Save favorites to file:
void FavoriteJobs::saveFavorites() {
    std::ofstream file(filePath, std::ios::trunc);
    if (!file) {
        throw std::runtime_error("Could not open file for writing");
    }
    json j = json::object();
    for (const auto& [id, job] : favorites) {
        j[id] = {
            {"title", job.title},
            {"company", job.company},
            {"location", job.location},
            {"url", job.url},
            {"salary", job.salary},
            {"description", job.description},
            {"created_date", job.created_date},
            {"is_starred", job.is_starred}
        }; 
    }
    file << j.dump(4);
}


