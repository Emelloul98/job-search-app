#include "FavoriteJobs.h"

json jobToJson(const Job& job) {
    return {
        {"title", job.title},
        {"company", job.company},
        {"location", job.location},
        {"url", job.url},
        {"salary", job.salary},
        {"description", job.description},
        {"created_date", job.created_date},
        {"id", job.id},
        {"is_expanded", job.is_expanded},
        {"is_starred", job.is_starred}
    };
}

FavoriteJobs::FavoriteJobs(const std::string& path) : filePath(path) {
    loadFavorites();
}

void FavoriteJobs::loadFavorites() {
    try {
        std::ifstream file(filePath);
        if (file.is_open()) {
            favorites.clear();
            file >> favorites;
            file.close();
        }
        else {
            favorites = json::array();
            saveFavorites();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading file: " << e.what() << std::endl;
        favorites = json::array();
    }
}

void FavoriteJobs::saveFavorites() { //mabye with thread
    try {
        std::ofstream file(filePath);
        if (file.is_open()) {
            file << std::setw(4) << favorites << std::endl;
            file.close();
        }
        else {
            std::cerr << "Cannot open file for writing" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving file: " << e.what() << std::endl;
    }
}


bool FavoriteJobs::addJob(const Job& job) {
    json jobJson = jobToJson(job);
    favorites.push_back(jobJson);
    saveFavorites();
    return true;
}

json FavoriteJobs:: getFavorites() {
    return favorites;
}

bool FavoriteJobs::isJobInFavorites(const std::string& jobId) const {
    return std::any_of(favorites.begin(), favorites.end(),
        [&jobId](const json& jobJson) {
            return jobJson["id"] == jobId;
        });
}

bool FavoriteJobs::removeJob(const std::string& jobId) {
    auto it = std::remove_if(favorites.begin(), favorites.end(),
        [&jobId](const json& jobJson) {
            return jobJson["id"] == jobId;
        });

    if (it != favorites.end()) {
        favorites.erase(it, favorites.end());
        saveFavorites();
        return true;
    }
    return false;
}

