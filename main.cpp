#include "Threads/includes.hpp"
#include <iostream>
#include <thread>
#include <windows.h>
#include <string>
#include <ctime>
#include <vector>
#include "chrono"
#include "aitj/corenet.h"
#include "aitj/entry_auth.h"
#include "aitj/encrypter.h"

using namespace KeyAuth;
std::string name = Crypt("fortnite slooted").decrypt(); // App name
std::string ownerid = Crypt("gWrAx325nE").decrypt(); // Account ID
std::string secret = Crypt("a11071f0d6342aa89383f6956170bbc24ca6677d4b3917889fb4f3a54738d1c6").decrypt(); // Application secret (not used in latest C++ example)
std::string version = Crypt("1.0").decrypt(); // Application version. Used for automatic downloads see video here https://www.youtube.com/watch?v=kW195PLCBKs
std::string url = Crypt("https://keyauth.win/api/1.3/").decrypt(); // change if using KeyAuth custom domains feature
std::string path = ""; // (OPTIONAL) see tutorial here https://www.youtube.com/watch?v=I9rxt821gMk&t=1s

api KeyAuthApp(name, ownerid, version, url, path);
namespace SophieGain {
    static auto g_PML4Thread() -> void {
        auto LatestGWorld = ReadMemory<uintptr_t>(virtualaddy + UWORLD);
        while (true) {
            Timer timer;
            timer.start();
            auto array = ReadMemory<uintptr_t>(LatestGWorld + UWORLD);

            // Explicitly cast LatestGWorld to UWorld* before comparison
            if ((LatestGWorld) != pointers.uworld) {
                auto cr3 = PagefaultAPI::FetchDtb();
                LatestGWorld = ReadMemory<uintptr_t>(virtualaddy + UWORLD);
            }

            timer.stop();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }


}




void entry_point() {

    printf("[main] Waiting for Fortnite...\n");


    std::cout << "[main] Initializing input\n";
    if (!input::init()) {
        std::cout << "[main] The input was not initialized :(\n";
        printf("The input was not initialized :(\n");
        Sleep(3000);
        exit(0);
    }
    std::cout << "[main] Input initialized successfully\n";

    std::cout << "[main] Initializing GUI\n";
    if (!gui::init()) {
        std::cout << "[main] The gui was not initialized :(\n";
        printf("The gui was not initialized :(\n");
        Sleep(3000);
        exit(0);
    }
    std::cout << ("[main] GUI initialized successfully\n");

    std::cout << "[main] Connecting to driver\n";
    if (!PagefaultAPI::AttachDriver()) {
        std::cout << "[main] Failed to connect to driver\n";
        Sleep(3500);
        exit(0);
    }
    std::cout << "[main] Driver connected successfully\n";

    std::cout << "[main] Getting process ID for Fortnite\n";
    PagefaultAPI::process_id = PagefaultAPI::find_process(L"FortniteClient-Win64-Shipping.exe");
    if (PagefaultAPI::process_id == 0) {
        std::cout << "[main] Failed to get Fortnite process ID\n";
        Sleep(3500);
        exit(0);
    }
    std::cout << "[main] Fortnite process ID: " << PagefaultAPI::process_id << "\n";

    std::cout << "[main] Getting Fortnite window\n";
    game_wnd = get_process_wnd(PagefaultAPI::process_id); // found the process
    if (!game_wnd) {
        std::cout << "[main] Failed to get Fortnite window\n";
        Sleep(3500);
        exit(0);
    }
    std::cout << "[main] Fortnite window found: " << game_wnd << "\n";

    std::cout << "[main] Getting base address\n";
    virtualaddy = PagefaultAPI::Fetch_ProcessBaseAddress();
    if (!virtualaddy) {
        std::cout << "[main] Failed to get base address\n";
        Sleep(3500);
        exit(0);
    }
    std::cout << "[main] Base address: 0x" << std::hex << virtualaddy << "\n";

    std::cout << "[main] Getting CR3\n";
    DWORD cat = PagefaultAPI::FetchDtb();
    if (cat == 0) {
        std::cout << "[main] No cr3!\n";
        Sleep(2000);
        exit(1);
    }
    std::cout << "[main] CR3: 0x" << std::hex << cat << "\n";
    std::thread([&]() { SophieGain::g_PML4Thread(); }).detach();
    std::thread([&]() { ViewPoint::g_CameraThread(); }).detach();
    std::thread([&]() { SophieRain::g_PointersThread(); }).detach();
    std::thread([&]() { SophieRain::g_CombatThread(); }).detach();
    std::thread([&]() { SophieRain::g_ExploitsThread(); }).detach();

    std::cout << "[main] Creating overlay\n";
    create_overlay();
    std::cout << "[main] Overlay created\n";

    std::cout << "[main] Initializing DirectX\n";
    Init();
    std::cout << "[main] DirectX initialized\n";

    std::cout << "[main] Entering main loop\n";
    Loop();
    std::cout << "[main] Exited main loop\n";
}




// [[ Note -> Make sure to remove the boolean checking on release, not good practise to keep it here! ]] 
void main() {
    if (SophieRain::debug_app) {
        entry_point();
    }

    else {
        SetConsoleTitleA(Crypt("Flare Slotted < developer: morva999"));
        KeyAuthApp.init();
        if (!KeyAuthApp.response.success)
        {
            ConsoleUtils::Write(KeyAuthApp.response.message);
            Sleep(2000);
            exit(0);
        }
        system("cls");
        ConsoleUtils::WriteLine("Locating server communication..");
        Sleep(2000);
        system("cls");
        Sleep(1500);
        ConsoleUtils::WriteLine("Success.");
        Sleep(1000);
        ConsoleUtils::WriteLine("Now loading preauthentication, please wait.");
        Sleep(2000);
        std::string key;
        ConsoleUtils::Write("Enter your key -> ");
        std::cin >> key;
        KeyAuthApp.license(key);

        if (!KeyAuthApp.response.success)
        {
            ConsoleUtils::Write(KeyAuthApp.response.message);
            Sleep(2000);
            exit(0);
        }
        ConsoleUtils::WriteLine("Welcome.");
        Sleep(2000);
        ConsoleUtils::WriteLine("Now loading.");
        Sleep(2000);
        system("cls");
        Sleep(5000);
        system("cls");
        ConsoleUtils::WriteLine("Connecting to overlay.");
        Sleep(1500);
        system("cls");
        ConsoleUtils::WriteLine("Connecting to process.");
        Sleep(1500);
        system("cls");
        entry_point();
    }

   
}





