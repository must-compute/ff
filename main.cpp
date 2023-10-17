#include "single_include/tui/tui.hpp"
#include <filesystem>
#include <vector>
#include <string>

std::vector<std::string> list_directory_relative_to_home() {
    std::vector<std::string> result;
    const auto home_path = std::filesystem::path(std::getenv("HOME"));
    const auto current_path = std::filesystem::current_path();
    for (const auto &entry: std::filesystem::directory_iterator(current_path)) {
        const auto relative_path = std::filesystem::relative(entry.path(), home_path);
        result.push_back("~/" + relative_path.string());
    }
    return result;
}


std::vector<std::string> list_directory() {
    std::vector<std::string> result;
    for (const auto &entry: std::filesystem::directory_iterator(std::filesystem::current_path())) {
        result.push_back(entry.path().string());
    }
    return result;
}

int main() {
    tui::Window window;

    window.set_title("ff");

    tui::List l;
    l.title = "results for SEARCH_QUERY in MY/CURRENT/PATH";
    l.rows = list_directory_relative_to_home();
    l.set_dimensions(0, 0, 80, 8);

    bool quit = false;
    tui::Event event;

    while (!quit) {
        if (window.poll_event(event)) {
            if (event.type == tui::KEYDOWN) {
                switch (event.key) {
                    case 'q':
                        quit = true;
                        break;
                    case 'j':
                        l.scroll_down(window);
                        break;
                    case 'k':
                        l.scroll_up(window);
                        break;
                }
            }
        }
        // Add list widget to the window
        // in the while loop to update
        // the list for any changes.
        window.add(l);
        window.render();
    }

    window.close();
    return 0;
}