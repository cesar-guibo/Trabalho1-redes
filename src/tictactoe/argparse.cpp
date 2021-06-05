#include "argparse.hpp"

#include <string_view>
#include <vector>

void ArgParser::add_default(std::string arg, std::string val) {
    opts[arg] = val;
}

void ArgParser::add_positional(std::string arg) {
    positionals.push_back(arg);
}

void ArgParser::parse(int argc, char *argv[]) {
    auto args = std::vector<std::string_view>(argv+1, argv+argc);
    auto last_pos = positionals.begin();

    std::string_view last;
    for (auto const& s : args) {
        if (last.empty()) {
            if (s.front() == '-') {
                last = std::string_view(s.begin() + 1, s.size() - 1);
            }
            else {
                opts[*last_pos] = s;
                ++last_pos;
            }
        }
        else {
            opts[std::string(last)] = s;
            last = std::string_view();
        }
    }
}

std::string ArgParser::get_arg(std::string arg) {
    return opts[arg];
}
