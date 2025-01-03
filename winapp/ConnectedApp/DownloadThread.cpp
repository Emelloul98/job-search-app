#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Recipe, name, cuisine, difficulty, cookTimeMinutes ,image)

void DownloadThread::operator()(CommonObjects& common)
{
	httplib::Client cli("https://dummyjson.com");

	auto res = cli.Get("/recipes");
	if (res->status == 200)
	{
		auto json_result = nlohmann::json::parse(res->body);
		common.recipies = json_result["recipes"];
		if (common.recipies.size() > 0)
			common.data_ready = true;
	}
}

void DownloadThread::SetUrl(std::string_view new_url)
{
	_download_url = new_url;
}



//int main() {
//    httplib::Client cli("https://api.adzuna.com");
//
//    std::string app_id = "40ab1a9c";
//    std::string app_key = "cf73de75ea3954c26f2dabdaa9f233ef";
//
//    // std::string keywords = "python,java";
//
//    auto res = cli.Get("https://api.adzuna.com/v1/api/jobs/us/top_companies?app_id=40ab1a9c&app_key=cf73de75ea3954c26f2dabdaa9f233ef%09&what=doctor");
//
//    if (res && res->status == 200) {
//        nlohmann::json response = nlohmann::json::parse(res->body);
//        std::cout << response.dump(4) << std::endl;
//    }
//    else {
//        std::cerr << "Error fetching job data: " << res.error() << std::endl;
//    }
//}