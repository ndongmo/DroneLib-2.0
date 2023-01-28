#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include <utils/Config.h>

using namespace utils;

const char *INT_NAME = "int_name";
const char *STRING_NAME = "string_name";

const int INT_VALUE = 106;
const char *STRING_VALUE = "string_value";

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream configFile(CONFIG_FILE);
        configFile << "{"
            << "\"" << INT_NAME << "\":" << INT_VALUE << ","
            << "\"" << STRING_NAME << "\":\"" << STRING_VALUE << "\""
            << "}";
        configFile.close();
        Config::init();
    }

    void TearDown() override {
        remove(CONFIG_FILE);
    }
};

// Tests Config get/set returning expected value
TEST_F(ConfigTest, GetIntReturnExpectedValue) {
    int var1, var2;
    std::string var3, var4;

    EXPECT_EQ(Config::getInt(INT_NAME, 0), INT_VALUE);
    EXPECT_EQ(Config::getString(STRING_NAME, ""), STRING_VALUE);
    EXPECT_TRUE(Config::setInt(INT_NAME, var1));
    EXPECT_TRUE(Config::setString(STRING_NAME, var3));
    EXPECT_EQ(Config::get(INT_NAME, 0), INT_VALUE);
    EXPECT_EQ(Config::get(STRING_NAME, std::string("")), STRING_VALUE);
    EXPECT_TRUE(Config::set(INT_NAME, var2));
    EXPECT_TRUE(Config::set(STRING_NAME, var4));
    EXPECT_EQ(var1, INT_VALUE);
    EXPECT_EQ(var3, STRING_VALUE);
    EXPECT_EQ(var2, INT_VALUE);
    EXPECT_EQ(var4, STRING_VALUE);
}

// Tests Config get/set returning expected value
TEST_F(ConfigTest, GetIntVarReturnExpectedValue) {
    Config::setIntVar(INT_NAME, INT_VALUE);
    Config::setStringVar(STRING_NAME, STRING_VALUE);

    EXPECT_EQ(Config::getIntVar(INT_NAME), INT_VALUE);
    EXPECT_EQ(Config::getStringVar(STRING_NAME), STRING_VALUE);
}

// Tests Config get/set returning unexpected value
TEST_F(ConfigTest, GetIntReturnUnexpectedValue) {
    int var1, var2;
    int def1 = 35;
    std::string var3, var4;
    std::string def2 = "default";
    const char *NOT_EXIST = "not_exist";

    EXPECT_EQ(Config::getInt(NOT_EXIST, def1), def1);
    EXPECT_EQ(Config::getString(NOT_EXIST, def2), def2);
    EXPECT_FALSE(Config::setInt(NOT_EXIST, var1));
    EXPECT_FALSE(Config::setString(NOT_EXIST, var3));
    EXPECT_EQ(Config::get(NOT_EXIST, def1), def1);
    EXPECT_EQ(Config::get(NOT_EXIST, def2), def2);
    EXPECT_FALSE(Config::set(NOT_EXIST, var2));
    EXPECT_FALSE(Config::set(NOT_EXIST, var4));
}