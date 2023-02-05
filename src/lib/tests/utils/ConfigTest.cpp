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
TEST_F(ConfigTest, GetReturnExpectedValue) {
    EXPECT_EQ(Config::getInt(INT_NAME), INT_VALUE);
    EXPECT_EQ(Config::getString(STRING_NAME), STRING_VALUE);
}

// Tests Config get/set returning unexpected value
TEST_F(ConfigTest, GetReturnUnexpectedValue) {
    EXPECT_EQ(Config::getInt(STRING_VALUE), 0);
    EXPECT_EQ(Config::getString(STRING_VALUE), "");
}

// Tests Config encode/decode json return expected value
TEST_F(ConfigTest, EncodeAndDecodeJson) {
    int eivar1 = 234, eivar2 = 98783, eivar3 = 0;
    std::string esvar1 = "my value to is : not ", esvar2 = ";:sisf", esvar3 = "";
    std::string ikey1 = "KEY1", ikey2 = "SssS:EG", ikey3 = "KEY3 SF";
    std::string skey1 = "23SKEY1", skey2 = "2SsffsS:EG", skey3 = "KEY3sfdsf2 SF";

    Config::setInt(ikey1.c_str(), eivar1);
    Config::setInt(ikey2.c_str(), eivar2);
    Config::setInt(ikey3.c_str(), eivar3);

    Config::setString(skey1.c_str(), esvar1);
    Config::setString(skey2.c_str(), esvar2);
    Config::setString(skey3.c_str(), esvar3);

    std::string estr = Config::encodeJson({ikey1, ikey2, ikey3, skey1, skey2, skey3});
    std::string exp = "{\"" + 
        ikey1 + "\" : " + std::to_string(eivar1) + ",\"" +
        ikey2 + "\" : " + std::to_string(eivar2) + ",\"" +
        ikey3 + "\" : " + std::to_string(eivar3) + ",\"" +
        skey1 + "\" : \"" + esvar1 + "\",\"" +
        skey2 + "\" : \"" + esvar2 + "\",\"" +
        skey3 + "\" : \"" + esvar3 +
    "\"}";

    EXPECT_EQ(estr, exp);

    Config::setInt(ikey1.c_str(), 0);
    Config::setInt(ikey2.c_str(), 0);
    Config::setInt(ikey3.c_str(), 0);

    Config::setString(skey1.c_str(), "");
    Config::setString(skey2.c_str(), "");
    Config::setString(skey3.c_str(), "");

    EXPECT_EQ(Config::decodeJson(estr), 1);
    EXPECT_EQ(Config::getInt(ikey1.c_str()), eivar1);
    EXPECT_EQ(Config::getInt(ikey2.c_str()), eivar2);
    EXPECT_EQ(Config::getInt(ikey3.c_str()), eivar3);
    EXPECT_EQ(Config::getString(skey1.c_str()), esvar1);
    EXPECT_EQ(Config::getString(skey2.c_str()), esvar2);
    EXPECT_EQ(Config::getString(skey3.c_str()), esvar3);
}