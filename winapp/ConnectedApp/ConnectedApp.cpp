// ConnectedApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <thread>
#include "CommonObject.h"
#include "DrawThread.h"
#include "DownloadThread.h"
#include <csignal>
int main()
{
    CommonObjects common;
    DrawThread draw;
	// Create a thread to run the draw function:
    auto draw_th = std::jthread([&] {draw(common); });
    DownloadThread down;
	// Create a thread to run the download function:
    auto down_th = std::jthread([&] {down(common); });
    std::cout << "running...\n";
	// Wait for the threads to finish:
    down_th.join();
    draw_th.join();
	std::cout << "done\n";
}


