#pragma once
#include <atomic>
#include <string>
#include <vector>
#include <unordered_map>

//Defines the movie structure to hold movie details
struct movie {
    int rank;
    std::string title;
    std::string description;
    int year;
    std::string rating;
    std::vector<std::string> genre; // Store genres as a vector of strings
};

//Defines the CommonObjects struct, which contains shared data used by multiple threads.
struct CommonObjects {
    std::atomic_bool exit_flag = false; // Flag to signal exit
    std::atomic_bool start_download = false; // Flag to signal start of download
    std::atomic_bool data_ready = false; // Flag to signal data readiness
    std::string url; // URL for downloading data
    std::vector<movie> movies; // Full list of movies
    std::vector<movie> filtered_movies; // Filtered list for display
    int selected_movie_index = -1; // Index of the selected movie
    std::unordered_map<std::string, bool> favorites; // Map to store favorite movies
    std::string current_genre; // Current selected genre
};