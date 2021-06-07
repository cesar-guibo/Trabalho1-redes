#include "serializable.hpp"
#include <cstring>

Serializable::Serializable() {}

std::string Serializable::serialize()
{return std::string("");};

std::vector<std::string> Serializable::split_fields(std::string serialized)
{
    std::vector<std::string> substrs;
    int start = 0;
    int end;
    do {
        const char *delimiter = serialized[start] == '{'
            ? "}," : serialized[start] == '[' ? "]," : ",";
        int delimiter_size = strlen(delimiter);
        end = serialized.find(delimiter, start);
        int substring_size = end - start + delimiter_size - 1;
        substrs.push_back(serialized.substr(start, substring_size));
        start = end + delimiter_size;
    } while (end != std::string::npos);
    return substrs;
}
