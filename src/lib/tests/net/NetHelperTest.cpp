#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <net/NetHelper.h>

using namespace net;

// Tests writing and reading int16
TEST(NetHelperTest, writeReadUInt16Work) {
    UINT8 buf[2];
    UINT16 test = 356;

    NetHelper::writeUInt16(test, buf, 0);
    EXPECT_EQ(NetHelper::readUInt16(buf, 0), test);
}

// Tests writing and reading int32
TEST(NetHelperTest, writeReadUInt32Work) {
    UINT8 buf[4];
    UINT32 test = 4587;

    NetHelper::writeUInt32(test, buf, 0);
    EXPECT_EQ(NetHelper::readUInt32(buf, 0), test);
}

// Tests reading string
TEST(NetHelperTest, readStringWorks) {
    const char* buf = "readStringWork";

    EXPECT_EQ(NetHelper::readString(buf, 0), "readStringWork");
}

// Tests reading frame
TEST(NetHelperTest, readFrameWork) {
    int id = 2;
    int type = 44;
    int seq = 99;
    int totalSize = 124;
    UINT8 buffer[totalSize];
    char data[] = "writeReadFrameWork";
    NetFrame rframe;
    rframe.data = buffer + NET_FRAME_HEADER;
    NetFrame wframe;
    wframe.type = type;
    wframe.id = id;
    wframe.seq = seq;
    wframe.size = totalSize;
    wframe.data = (UINT8*)data;

    buffer[0] = wframe.type;
	buffer[1] = wframe.id;
	buffer[2] = wframe.seq;
	NetHelper::writeUInt32(wframe.size, buffer, 3); 
    memcpy(&buffer[NET_FRAME_HEADER], wframe.data, wframe.size - NET_FRAME_HEADER);

    NetHelper::readFrame((char*)buffer, rframe);
    
    ASSERT_EQ(wframe.type, rframe.type);
    ASSERT_EQ(wframe.id, rframe.id);
    ASSERT_EQ(wframe.seq, rframe.seq);
    ASSERT_EQ(wframe.size, rframe.size);
    EXPECT_EQ(memcmp(wframe.data, rframe.data, wframe.size - NET_FRAME_HEADER), 0);

    const char* format = "1b24b";
    UINT8 rv1;
    int wv1 = 56;
    bool wv2 = true, rv2;
    int wv3 = 368, rv3;
    int wv4 = 7899, rv4;
    bool wv5 = false, rv5;

    memset(buffer, 0, totalSize);

    buffer[0] = wframe.type;
	buffer[1] = wframe.id;
	buffer[2] = wframe.seq;
	NetHelper::writeUInt32(wframe.size, buffer, 3);
    buffer[7] = (UINT8)wv1;
    buffer[8] = (UINT8)wv2;
    NetHelper::writeUInt16(wv3, buffer, 9);
    NetHelper::writeUInt32(wv4, buffer, 11);
    buffer[15] = (UINT8)wv5;

    NetHelper::readFrame((char*)buffer, rframe);
    NetHelper::readArgs(rframe, format, &rv1, &rv2, &rv3, &rv4, &rv5);

    ASSERT_EQ(wframe.type, rframe.type);
    ASSERT_EQ(wframe.id, rframe.id);
    ASSERT_EQ(wframe.seq, rframe.seq);
    ASSERT_EQ(wframe.size, rframe.size);
    ASSERT_EQ(wv1, rv1);
    ASSERT_EQ(wv2, rv2);
    ASSERT_EQ(wv3, rv3);
    ASSERT_EQ(wv4, rv4);
    ASSERT_EQ(wv5, rv5);
}