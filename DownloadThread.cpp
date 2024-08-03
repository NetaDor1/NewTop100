#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

//Implements the DownloadThread class, which contains the logic for downloading and parsing movie data from an API.
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(movie, rank, title, description, year, rating, genre);


void parse_and_add_genres(const nlohmann::json& json_genre_array, std::vector<std::string>& genres) {
    for (auto& item : json_genre_array) {
        genres.push_back(item.get<std::string>());
    }
}


// function handles the actual download and JSON parsing,
// updating the CommonObjects with the retrieved movie data.
void DownloadThread::operator()(CommonObjects& common) {
    auto rapidpi_key = "dd57e4c034msh78b035a7e7c5325p112114jsnfdd12fa6860d"; // API key for RapidAPI
    auto host = "https://imdb-top-100-movies.p.rapidapi.com"; // Base URL for the API

    httplib::Client cli(host); // Create an HTTP client for the specified host
    httplib::Headers headers = { { "x-rapidapi-key",rapidpi_key} }; // Set the headers, including the API key


    auto res = cli.Get("/", headers); // bring the data
    if (res->status == 200) { // Check if the request was successful
        auto json_result = nlohmann::json::parse(res->body);
        for (auto& item : json_result) {
            movie new_movie;
            new_movie.rank = item["rank"];
            new_movie.title = item["title"];
            new_movie.description = item["description"];
            new_movie.year = item["year"];
            new_movie.rating = item["rating"];
            parse_and_add_genres(item["genre"], new_movie.genre);  // Assume 'genre' is the JSON key for genres
            common.movies.push_back(new_movie);
        }
        if (!common.movies.empty())
            common.data_ready = true;
    }
}

//SetUrl function allows setting the URL for downloading movie data.
void DownloadThread::SetUrl(std::string_view new_url) {
    _download_url = new_url;
}