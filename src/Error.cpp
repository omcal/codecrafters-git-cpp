//
// Created by joi on 12.04.2024.
//

#include "Error.h"

void Error::SetMessage(std::string str) {
    message=str;
}

std::string Error::GetMessage() {
    return message;
}

void Error::SetBool(bool bl) {
    Error=bl;

}

bool Error::GetBool() {
    return Error;
}
