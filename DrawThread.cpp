#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <algorithm>  // Include for std::transform
#include <cctype>     // Include for std::tolower
//Implements the DrawThread class, which contains the GUI logic using ImGui.

//DrawHeart function is used to draw a heart icon for favorite movies.
void DrawHeart(ImDrawList* draw_list, ImVec2 pos, float size, bool isFavorite) {
    ImVec2 center = pos;
    ImVec2 leftCircleCenter = ImVec2(center.x - size * 0.3f, center.y - size * 0.3f);
    ImVec2 rightCircleCenter = ImVec2(center.x + size * 0.3f, center.y - size * 0.3f);
    ImVec2 triangleBottom = ImVec2(center.x, center.y + size * 0.5f);

    if (isFavorite) {
        // Draw filled red heart
        draw_list->AddCircleFilled(leftCircleCenter, size * 0.3f, IM_COL32(255, 0, 0, 255), 20);
        draw_list->AddCircleFilled(rightCircleCenter, size * 0.3f, IM_COL32(255, 0, 0, 255), 20);
        draw_list->AddTriangleFilled(leftCircleCenter, rightCircleCenter, triangleBottom, IM_COL32(255, 0, 0, 255));
    }
    else {
        // Draw filled white heart
        draw_list->AddCircleFilled(leftCircleCenter, size * 0.3f, IM_COL32(255, 255, 255, 255), 20);
        draw_list->AddCircleFilled(rightCircleCenter, size * 0.3f, IM_COL32(255, 255, 255, 255), 20);
        draw_list->AddTriangleFilled(leftCircleCenter, rightCircleCenter, triangleBottom, IM_COL32(255, 255, 255, 255));
    }
}

//DrawAppWindow function is the main function for drawing the application window,
void DrawAppWindow(void* common_ptr) {
    auto common = static_cast<CommonObjects*>(common_ptr);
    ImGui::Begin("Top 100 movies");

    static char buff[200]; // Buffer to hold search text
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White background for the search box
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black text for the search box
    ImGui::InputTextWithHint("##search", "Search Movie", buff, sizeof(buff));
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    if (ImGui::Button("Search")) {
        std::string searchQuery = buff; // Store the content of the buffer (search text) into a string
        common->filtered_movies.clear();
        if (!searchQuery.empty()) {
            std::transform(searchQuery.begin(), searchQuery.end(), searchQuery.begin(),
                [](unsigned char c) { return std::tolower(c); });  // Convert the search query to lowercase
            for (const auto& movie : common->movies) { //Converts to lowercase letters
                std::string titleLower = movie.title; // Get the movie title
                std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(),
                    [](unsigned char c) { return std::tolower(c); }); // Convert the movie title to lowercase
                if (titleLower.find(searchQuery) != std::string::npos) { //if found
                    common->filtered_movies.push_back(movie);
                }
            }
        }
    }

    ImGui::SameLine();
    // My Favorites Button
    if (ImGui::Button("My Favorites")) {
        common->filtered_movies.clear();
        for (const auto& movie : common->movies) {
            if (common->favorites[movie.title]) {
                common->filtered_movies.push_back(movie);
            }
        }
    }
    // Calculate the necessary offset to place the Back button at the right end of the window
    float availableSpace = ImGui::GetContentRegionAvail().x;  // Get available space on the line
    ImGui::SameLine(availableSpace - 80);  // Position the Back button at the end of the available space

    if (ImGui::Button("Back", ImVec2(70, 20))) {
        // Clear the search buffer and reset the filtered movies
        memset(buff, 0, sizeof(buff));  // Optionally clear the search text
        common->filtered_movies.clear();
        common->filtered_movies = common->movies;  // Show full list
    }
    // Genre buttons
    std::vector<std::string> genres = { "Action", "Adventure", "Animation", "Biography", "Comedy", "Crime", "Drama", "Family", "Fantasy", "Film-Noir", "History", "Horror", "Music", "Musical", "Mystery", "Romance", "Sci-Fi", "Thriller", "War", "Western" };
    int buttonCount = 0;
    ImVec2 buttonSize(80, 20); // same size to all genre buttons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 2)); // Small  spacing
    for (const auto& genre : genres) {
        // Set button and text colors
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f)); //color's button
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White text


        if (ImGui::Button(genre.c_str(), buttonSize)) { //sort by genre
            common->current_genre = genre;
            common->filtered_movies.clear();
            for (const auto& movie : common->movies) {
                // Check if the movie's genre list contains the clicked genre
                if (std::find(movie.genre.begin(), movie.genre.end(), genre) != movie.genre.end()) {
                    common->filtered_movies.push_back(movie);
                }
            }
        }
        ImGui::PopStyleColor(2);  // Pop both the button and text color styles
        if (++buttonCount % 10 != 0) { //keep 10 buttons in the same line
            ImGui::SameLine();
        }
    }
    ImGui::PopStyleVar(); // Restore spacing style

    // Decide which list to display
    const auto& display_movies = common->filtered_movies.empty() ? common->movies : common->filtered_movies;

    if (common->data_ready) {
        if (ImGui::BeginTable("Movies", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
            // Set up columns
            ImGui::TableSetupColumn("Rank", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthFixed, 400.0f);
            ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Rating", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Genre", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Favorite", ImGuiTableColumnFlags_None);

            // Calculate and adjust header positions
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            for (int col = 0; col < 7; col++) {
                ImGui::TableSetColumnIndex(col);
                float columnWidth = ImGui::GetColumnWidth();
                const char* name = ImGui::TableGetColumnName(col); // Get the name of the column
                float textWidth = ImGui::CalcTextSize(name).x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - textWidth) / 2);
                ImGui::TableHeader(name);
            }

            for (int i = 0; i < display_movies.size(); ++i) {
                auto& movie = display_movies[i];
                ImGui::PushID(i); // sets a unique ID for each row.
                ImGui::TableNextRow();

                // Rank column centered
                ImGui::TableSetColumnIndex(0);
                float columnWidth = ImGui::GetColumnWidth();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - ImGui::CalcTextSize(std::to_string(movie.rank).c_str()).x) / 2);
                ImGui::Text("%d", movie.rank);

                // Title column left-aligned
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", movie.title.c_str());

                // Year column centered
                ImGui::TableSetColumnIndex(2);
                columnWidth = ImGui::GetColumnWidth();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - ImGui::CalcTextSize(std::to_string(movie.year).c_str()).x) / 2);
                ImGui::Text("%d", movie.year);

                // Rating column centered
                ImGui::TableSetColumnIndex(3);
                columnWidth = ImGui::GetColumnWidth();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - ImGui::CalcTextSize(movie.rating.c_str()).x) / 2);
                ImGui::Text("%s", movie.rating.c_str());

                ImGui::TableSetColumnIndex(4);
                std::string genre_str;
                for (const auto& genre : movie.genre) { //if there more than one genre
                    if (!genre_str.empty()) genre_str += ", ";
                    genre_str += genre;
                }
                ImGui::Text("%s", genre_str.c_str());

                // Show Description button centered
                ImGui::TableSetColumnIndex(5);
                columnWidth = ImGui::GetColumnWidth();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - ImGui::CalcTextSize("Show").x) / 2);
                if (ImGui::Button("Show")) {
                    common->selected_movie_index = i;
                    ImGui::OpenPopup("Movie Description");
                }

                // Favorite heart centered
                ImGui::TableSetColumnIndex(6);
                bool is_favorite = common->favorites[movie.title];
                ImVec2 pos = ImGui::GetCursorScreenPos();
                float height = ImGui::GetTextLineHeight();
                columnWidth = ImGui::GetColumnWidth();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                if (ImGui::InvisibleButton("fav", ImVec2(columnWidth, height))) { //Invisible Button for change the heart
                    common->favorites[movie.title] = !is_favorite;
                }
                ImVec2 heartPos = ImVec2(pos.x + (columnWidth - height) * 0.5f, pos.y);
                DrawHeart(draw_list, ImVec2(heartPos.x + height * 0.5f, heartPos.y + height * 0.5f), height * 0.5f, is_favorite);

                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    // Modal for displaying the description
    if (common->selected_movie_index != -1) {
        ImGui::OpenPopup("Movie Description");
    }

    if (ImGui::BeginPopupModal("Movie Description", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (common->selected_movie_index != -1) {
            const auto& selected_movie = display_movies[common->selected_movie_index];
            ImGui::SetWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);
            ImGui::BeginChild("Description Text", ImVec2(480, 250), true);
            ImGui::TextWrapped("%s", selected_movie.description.c_str());
            ImGui::EndChild();
            if (ImGui::Button("Close", ImVec2(120, 0))) { //Closes the popup when the "Close" button is clicked
                ImGui::CloseCurrentPopup();
                common->selected_movie_index = -1;
            }
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}


//operator() function runs the GUI main loop and updates the exit_flag when the GUI is closed.
void DrawThread::operator()(CommonObjects& common) {

    GuiMain(DrawAppWindow, &common);
    common.exit_flag = true;
}