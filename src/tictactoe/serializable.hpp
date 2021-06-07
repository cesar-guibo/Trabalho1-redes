#ifndef CSV_SERIALIZABLE_H
#define CSV_SERIALIZABLE_H

#include <string>
#include <vector>
#include <stack>

class DelimitersTracker {
    public:
        DelimitersTracker();
        bool is_field_end_delimiter(char ch);

    private:
        std::stack<char> open_delimiters;
};

class Serializable {
    public:
        Serializable();
        virtual std::string serialize();
        static std::vector<std::string> split_fields(std::string serialized);
};



#endif
