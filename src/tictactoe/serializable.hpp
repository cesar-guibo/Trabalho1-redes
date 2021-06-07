#ifndef CSV_SERIALIZABLE_H
#define CSV_SERIALIZABLE_H

#include <string>
#include <vector>

class Serializable {
    public:
        Serializable();
        virtual std::string serialize();
        static std::vector<std::string> split_fields(std::string serialized);
};



#endif
