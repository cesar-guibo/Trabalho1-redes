#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <map>
#include <string>
#include <vector>
#include <string_view>

class ArgParser {
public:
    ArgParser() = default;
    void add_default(std::string arg, std::string val);
    void add_positional(std::string arg);
    void parse(int argc, char *argv[]);

    std::string get_arg(std::string);

private:
    std::map<std::string, std::string> opts;
    std::vector<std::string> positionals;
};

#endif