#include <filesystem>
#include <vector>
#include <string>
#include <ncurses.h>
#include <optional>
#include <fstream>
#include <sstream>
#include <unordered_map>

std::vector<std::string> list_directory() {
    std::vector<std::string> result;
    for (const auto &entry: std::filesystem::directory_iterator(std::filesystem::current_path())) {
        result.push_back(entry.path().string());
    }
    return result;
}

std::unordered_map<std::string, std::optional<std::string>> read_directory_files() {
    std::unordered_map<std::string, std::optional<std::string>> result;
    const auto current_path = std::filesystem::current_path();

    for (const auto &entry: std::filesystem::directory_iterator(current_path)) {
        if (std::filesystem::is_regular_file(entry.status())) {
            std::ifstream file(entry.path(), std::ios::binary);
            if (file) {
                std::stringstream ss;
                ss << file.rdbuf();
                result[entry.path().string()] = ss.str();
            } else {
                result[entry.path().string()] = std::nullopt;
            }
        }
    }
    return result;
}

int main() {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    auto itemContent = read_directory_files();
    std::vector<std::string> items;

    items.reserve(itemContent.size());
    for (const auto &pair: itemContent) {
        items.push_back(pair.first);
    }

    int highlight = 0;
    int choice;
    int offset = 0;
    const int MAX_DISPLAY = 4;

    while (1) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        auto position_status = std::to_string(highlight).append("/").append(
                std::to_string(items.size() - 1)).c_str();

        for (int i = 0; i < MAX_DISPLAY; ++i) {
            if (i + offset == highlight) {
                attron(A_REVERSE);
            }
            mvprintw(i, 0, items[i + offset].c_str());
            attroff(A_REVERSE);
        }

        mvhline(MAX_DISPLAY, 0, 0, max_x - strlen(position_status) - 1);
        mvprintw(MAX_DISPLAY, max_x - strlen(position_status), position_status);

        auto content = itemContent[items[highlight]] ? *itemContent[items[highlight]] : "NOT A TEXT FILE";
        mvprintw(MAX_DISPLAY + 1, 0, "Preview: %s", content.substr(0, 1000).c_str());

        choice = getch();

        switch (choice) {
            case KEY_UP:
                if (highlight > 0) {
                    --highlight;
                    if (highlight < offset) {
                        --offset;
                    }
                }
                clear();
                refresh();
                break;
            case KEY_DOWN:
                if (highlight < items.size() - 1) {
                    ++highlight;
                    if (highlight - offset >= MAX_DISPLAY) {
                        ++offset;
                    }
                }
                clear();
                refresh();
                break;
            case 10: // Enter key
                endwin();
                std::system(("vim " + items[highlight]).c_str());
                initscr();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                break;
            default:
                break;
        }

        if (choice == 'q') break;
    }

    endwin();
    return 0;
}
