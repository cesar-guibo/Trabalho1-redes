#include "serializable.hpp"
#include <cstring>
#include <sstream>
#include <stack>
#include <stdexcept>

DelimitersTracker::DelimitersTracker() {};

bool DelimitersTracker::is_field_end_delimiter(char ch)
{
    if (ch == ',')
        return open_delimiters.empty();
    if (ch == '{' or ch == '[')
        open_delimiters.push(ch);
    /* In ASCII, we have {: 123, {: 125 and [: 91, ]: 93 */
    if (!open_delimiters.empty() and ch - (char)2 == open_delimiters.top()) {
        open_delimiters.pop();
    }
    return false;
}


Serializable::Serializable() {}

std::string Serializable::serialize()
{return std::string("");};

std::vector<std::string> Serializable::split_fields(std::string serialized)
{
    if ((serialized.front() != '{' and serialized.front() != '[')
            or (serialized.back() != '}' and serialized.back() != ']')) {
        throw std::invalid_argument("Invalid serialized string");
    }
    serialized = serialized.substr(1, serialized.size() - 2);

    std::vector<std::string> substrs;
    DelimitersTracker tracker;
    std::stringstream ss;
    for (char ch : serialized) {
        if (tracker.is_field_end_delimiter(ch)) {
            substrs.push_back(ss.str());
            ss.str("");
        } else {
            ss << ch;
        }
    }
    substrs.push_back(ss.str());
    return substrs;
}
