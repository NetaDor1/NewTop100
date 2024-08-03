#include <iostream>
#include <thread>
#include "CommonObject.h"
#include "DrawThread.h"
#include "DownloadThread.h"

//Creates instances of CommonObjects, DrawThread, and DownloadThread.

int main() {
    CommonObjects common; // Create shared objects
    DrawThread draw; // Create draw thread object
    auto draw_th = std::jthread([&] {draw(common); }); // Start draw thread
    DownloadThread down; // Create download thread object
    auto down_th = std::jthread([&] {down(common); }); // Start download thread
    down.SetUrl("http://...."); // Set the URL for downloading data
    std::cout << "running...\n";
    down_th.join(); // Wait for download thread to finish
    draw_th.join(); // Wait for draw thread to finish
}