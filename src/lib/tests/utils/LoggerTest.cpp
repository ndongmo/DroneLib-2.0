#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <utils/Logger.h>

const char *WARNING = "warning";
const char *ERROR = "error";
const char *INFO = "info";

// Tests Logger redirect operator for file output
TEST(LoggerTest, UsingLogAddToFile) {
    logW << WARNING << std::endl;
    logE << ERROR << std::endl;
    logI << INFO << std::endl;

    std::ifstream input(LOG_FILE);
    std::string line;
    
    EXPECT_FALSE(input.fail());
    std::getline(input, line);
    ASSERT_NE(line.find(WARNING), std::string::npos);
    std::getline(input, line);
    ASSERT_NE(line.find(ERROR), std::string::npos);
    std::getline(input, line);
    ASSERT_NE(line.find(INFO), std::string::npos);
}

// Tests Config get/set returning unexpected value
TEST(LoggerTest, UsingLogAddToStdout) {
    std::stringstream buffer;
    std::string line;

    utils::Logger::init(buffer);
    
    logW << WARNING << std::endl;
    logE << ERROR << std::endl;
    logI << INFO << std::endl;

    std::getline(buffer, line);
    ASSERT_NE(line.find(WARNING), std::string::npos);
    std::getline(buffer, line);
    ASSERT_NE(line.find(ERROR), std::string::npos);
    std::getline(buffer, line);
    ASSERT_NE(line.find(INFO), std::string::npos);
}