#include "FavoriteJobs.h"

// המרה מ-Job ל-JSON
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
    /*if (isJobInFavorites(job.id)) {
        return false;
    }*/
    json jobJson = jobToJson(job);
    favorites.push_back(jobJson);
    saveFavorites();
    return true;
}

json FavoriteJobs:: getFavorites() {
    return favorites;
}

//bool FavoriteJobs::removeJob(const std::string& jobId) {
//    auto it = std::remove_if(favorites.begin(), favorites.end(),
//        [&jobId](const Job& job) { return job.id == jobId; });
//
//    if (it != favorites.end()) {
//        favorites.erase(it, favorites.end());
//        saveFavorites();
//        return true;
//    }
//    return false;
//}
//
//bool FavoriteJobs::isJobInFavorites(const std::string& jobId) const {
//    return std::any_of(favorites.begin(), favorites.end(),
//        [&jobId](const Job& job) { return job.id == jobId; });
//}


/*#include "FavoriteJobs.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// המרה מ-Job ל-JSON
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

// המרה מ-JSON ל-Job
Job jsonToJob(const json& j) {
    Job job;
    job.title = j["title"].get<std::string>();
    job.company = j["company"].get<std::string>();
    job.location = j["location"].get<std::string>();
    job.url = j["url"].get<std::string>();
    job.salary = j["salary"].get<std::string>();
    job.description = j["description"].get<std::string>();
    job.created_date = j["created_date"].get<std::string>();
    job.id = j["id"].get<std::string>();
    job.is_expanded = j["is_expanded"].get<bool>();
    job.is_starred = j["is_starred"].get<bool>();
    return job;
}

void FavoriteJobs::loadFavorites() {
    try {
        std::ifstream file(filePath);
        if (file.is_open()) {
            json j;
            file >> j;

            favorites.clear();
            for (const auto& jobJson : j) {
                favorites.push_back(jsonToJob(jobJson));
            }
            file.close();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading file: " << e.what() << std::endl;
        favorites.clear();
    }
}

void FavoriteJobs::saveFavorites() {
    try {
        std::ofstream file(filePath);
        if (file.is_open()) {
            json j = json::array();
            for (const auto& job : favorites) {
                j.push_back(jobToJson(job));
            }
            file << std::setw(4) << j << std::endl;
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

FavoriteJobs::FavoriteJobs(const std::string& path) : filePath(path) {
    loadFavorites();
}

bool FavoriteJobs::addJob(const Job& job) {
    if (isJobInFavorites(job.id)) {
        return false;
    }
    favorites.push_back(job);
    saveFavorites();
    return true;
}

const std::vector<Job>& FavoriteJobs::getAllFavorites() const {
    return favorites;
}

bool FavoriteJobs::removeJob(const std::string& jobId) {
    auto it = std::remove_if(favorites.begin(), favorites.end(),
        [&jobId](const Job& job) { return job.id == jobId; });

    if (it != favorites.end()) {
        favorites.erase(it, favorites.end());
        saveFavorites();
        return true;
    }
    return false;
}

bool FavoriteJobs::isJobInFavorites(const std::string& jobId) const {
    return std::any_of(favorites.begin(), favorites.end(),
        [&jobId](const Job& job) { return job.id == jobId; });
}*/