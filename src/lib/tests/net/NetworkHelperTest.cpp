#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <net/NetworkHelper.h>

using namespace net;

// Tests writing and reading int16
TEST(NetworkHelperTest, writeReadUInt16Work) {
    UINT8 buf[2];
    UINT16 test = 356;

    NetworkHelper::writeUInt16(test, buf, 0);
    EXPECT_EQ(NetworkHelper::readUInt16(buf, 0), test);
}

// Tests writing and reading int32
TEST(NetworkHelperTest, writeReadUInt32Work) {
    UINT8 buf[4];
    UINT32 test = 4587;

    NetworkHelper::writeUInt32(test, buf, 0);
    EXPECT_EQ(NetworkHelper::readUInt32(buf, 0), test);
}

// Tests reading string
TEST(NetworkHelperTest, readStringWorks) {
    const char* buf = "readStringWork";

    EXPECT_EQ(NetworkHelper::readString(buf, 0), "readStringWork");
}

// Tests reading string
TEST(NetworkHelperTest, writeReadFrameWork) {
    int id = 2;
    int type = 44;
    int seq = 99;
    int totalSize = 124;
    UINT8 buffer[totalSize];
    char data[] = "writeReadFrameWork";
    NetworkFrame rframe;
    NetworkFrame wframe;
    wframe.type = type;
    wframe.id = id;
    wframe.seq = seq;
    wframe.totalSize = totalSize;
    wframe.dataSize = totalSize - FRAME_HEADER;
    wframe.data = (UINT8*)data; 

    NetworkHelper::writeFrame(wframe, buffer);
    NetworkHelper::readFrame((char*)buffer, rframe);
    
    ASSERT_EQ(wframe.type, rframe.type);
    ASSERT_EQ(wframe.id, rframe.id);
    ASSERT_EQ(wframe.seq, rframe.seq);
    ASSERT_EQ(wframe.dataSize, rframe.dataSize);
    ASSERT_EQ(wframe.totalSize, rframe.totalSize);
    EXPECT_EQ(memcmp(wframe.data, rframe.data, wframe.dataSize), 0);

    const char* format = "1b24b";
    int wv1 = 56, rv1;
    bool wv2 = true, rv2;
    int wv3 = 368, rv3;
    int wv4 = 7899, rv4;
    bool wv5 = false, rv5;

    memset(buffer, 0, totalSize);
    NetworkHelper::writeFrame(wframe, buffer, format, wv1, wv2, wv3, wv4, wv5);
    NetworkHelper::readFrame((char*)buffer, rframe);
    NetworkHelper::readArgs(rframe, format, &rv1, &rv2, &rv3, &rv4, &rv5);

    ASSERT_EQ(wframe.type, rframe.type);
    ASSERT_EQ(wframe.id, rframe.id);
    ASSERT_EQ(wframe.seq, rframe.seq);
    ASSERT_EQ(wframe.dataSize, rframe.dataSize);
    ASSERT_EQ(wframe.totalSize, rframe.totalSize);
    ASSERT_EQ(wv1, rv1);
    ASSERT_EQ(wv2, rv2);
    ASSERT_EQ(wv3, rv3);
    ASSERT_EQ(wv4, rv4);
    ASSERT_EQ(wv5, rv5);
}