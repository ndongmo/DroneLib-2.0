#include "utils/Helper.h"
#include "utils/Config.h"
#include "utils/Constants.h"

#include <iostream>
#include <random>

using namespace utils;

std::string Helper::generateRandomString(int length) {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijk"
        "lmnopqrstuvwxyz0123456789";

    std::string random_string;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);
    
    for (int i = 0; i < length; ++i) {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}

std::string Helper::generateStreamUrl() {
    return 
        Config::getString(STREAM_PROTOCOL) + "://" +
        Config::getString(DRONE_ADDRESS) + ":" + 
        std::to_string(Config::getInt(DRONE_PORT_STREAM)) + "/" +
        generateRandomString(URL_KEY_LENGTH);
}