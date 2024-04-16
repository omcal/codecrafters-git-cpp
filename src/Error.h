//
// Created by joi on 12.04.2024.
//

#ifndef CODECRAFTERS_GIT_CPP_ERROR_H
#define CODECRAFTERS_GIT_CPP_ERROR_H
#include <string>

class Error {
public:
    bool Error= false;
    std::string message="";

    void SetBool(bool bl);

    bool GetBool();

    void SetMessage(std::string str);
    std::string GetMessage();
};


#endif //CODECRAFTERS_GIT_CPP_ERROR_H
