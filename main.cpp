#include <map>
#include <string>
#include <ncurses.h>
#include <optional>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>

// TODO consider capturing all matches within a line into a single search result
struct SearchResult {
    std::string filepath;
    int line_number{};
    std::string match;
};

struct RipgrepSearch {

    // TODO make sure to search both file names + file content (not just file content)
    static std::map<int, SearchResult> search(const std::string &pattern) {
        // the output we expect looks like this:
        // file1.txt:25
        // file2.txt:22
        // etc
        // rg reports multiple matches within the same line as separate results. Hence the pipe to `uniq`
        std::string cmd = "rg -o -n --smart-case --no-heading " + pattern + " | uniq";
        std::map<int, SearchResult> results;
        char buffer[128];
        FILE *fp = popen(cmd.c_str(), "r");
        int index = 0;

        if (fp == nullptr) {
            std::cerr << "Failed to run command\n";
            return {};
        }

        while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            std::string output = buffer;
            std::istringstream iss(output);
            SearchResult result;

            std::getline(iss, result.filepath, ':');
            std::string line_number_str;
            std::getline(iss, line_number_str, ':');
            result.line_number = std::stoi(line_number_str);
            std::getline(iss, result.match);

            results[index++] = result;
        }

        pclose(fp);
        return results;
    }
};

struct FFSearch {
    static std::map<int, SearchResult> search(const std::string &pattern) {
        // TODO implement me
        // ???
        return std::map<int, SearchResult>{};
    }
};

template<typename SearchPolicy>
struct SearchContext {
    std::map<int, SearchResult> run_search(const std::string &pattern) {
        SearchPolicy policy;
        return policy.search(pattern);
    }
};

int main(int argc, char *argv[]) {
    std::string query, policy;
    if (argc > 2) {
        // We currently assume only two args:
        //  1. the first arg specifies the search policy (rg or ff)
        //  1. the second arg specifies teh search query
        policy = argv[1];
        query = argv[2];
    } else {
        std::cerr << "Usage: " << "ff <policy> <query>" << std::endl;
        std::exit(1);
    }

    std::map<int, SearchResult> results{};
    if (policy == "rg") {
        SearchContext<RipgrepSearch> rgContext;
        results = rgContext.run_search(query);
    } else if (policy == "ff") {
        SearchContext<FFSearch> ffContext;
        results = ffContext.run_search(query);
    } else {
        std::cerr << "Usage: " << "ff <policy> <query>" << std::endl;
        std::exit(1);
    }

    // ncurses setup
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();

    // result file list setup
    int highlight = 0;
    int choice;
    int offset = 0;
    const int MAX_DISPLAY = 4;

    while (1) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        auto highlighted_result = results[highlight];

        auto position_status = std::to_string(highlight + 1).append("/").append(
                std::to_string(results.size())).c_str();

        // display scrollable result list
        // TODO handle case when results count < MAX_DISPLAY
        for (int i = 0; i < MAX_DISPLAY; ++i) {
            if (i + offset == highlight) {
                attron(A_REVERSE);
            }
            auto curr_result = results[i + offset];
            auto path_and_line_num = curr_result.filepath.append(":").append(std::to_string(curr_result.line_number));
            mvprintw(i, 0, path_and_line_num.c_str());
            attroff(A_REVERSE);
        }

        // display horizontal divider and scroll position
        mvhline(MAX_DISPLAY, 0, 0, max_x - strlen(position_status) - 1);
        mvprintw(MAX_DISPLAY, max_x - strlen(position_status), position_status);

        // preview file content of highlighted result:
        std::ifstream file(highlighted_result.filepath);
        std::string line;
        int current_line = 1;

        init_pair(1, COLOR_BLACK, COLOR_YELLOW);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_WHITE, COLOR_BLACK);

        if (file.is_open()) {
            while (std::getline(file, line) && current_line <= highlighted_result.line_number + 2) {
                if (current_line >= highlighted_result.line_number - 2) {
                    if (current_line == highlighted_result.line_number) {
                        attron(COLOR_PAIR(2) | A_BOLD);
                        printw("%02d ", current_line);
                        attroff(COLOR_PAIR(2) | A_BOLD);
                        std::string::size_type start = 0;
                        while ((start = line.find(highlighted_result.match, start)) != std::string::npos) {
                            std::string pre_match = line.substr(0, start);
                            std::string post_match = line.substr(start + highlighted_result.match.size());
                            line = post_match;
                            printw("%s", pre_match.c_str());
                            attron(COLOR_PAIR(1));
                            printw("%s", highlighted_result.match.c_str());
                            attroff(COLOR_PAIR(1));
                            start = 0;
                        }
                        printw("%s", line.c_str());
                    } else {
                        attron(COLOR_PAIR(3) | A_BOLD | A_DIM);
                        printw("%02d ", current_line);
                        attroff(COLOR_PAIR(3) | A_BOLD | A_DIM);
                        printw("%s", line.c_str());
                    }
                    printw("\n");
                }
                ++current_line;
            }
            file.close();
        }
        // end result preview

        // handle keyboard input:
        choice = getch();

        if (choice == KEY_UP) {
            if (highlight > 0) {
                --highlight;
                if (highlight < offset) {
                    --offset;
                }
            }
            clear();
            refresh();
        } else if (choice == KEY_DOWN) {
            if (highlight < results.size() - 1) {
                ++highlight;
                if (highlight - offset >= MAX_DISPLAY) {
                    ++offset;
                }
            }
            clear();
            refresh();
        } else if (choice == 10 /* Enter key */ ) {
            // TODO refresh search results when back from vim
            endwin();
            std::string vim_cmd = std::string("vim +").append(
                    std::to_string(highlighted_result.line_number)).append(" ").append(highlighted_result.filepath);
            std::system(vim_cmd.c_str());
            initscr();
            noecho();
            keypad(stdscr, TRUE);
            curs_set(0);
        } else if (choice == 'q') break;
    }
    // end handle keyboard input

    endwin();
    return 0;
}
