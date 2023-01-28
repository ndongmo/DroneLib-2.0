#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <net/NetHelper.h>
#include <stream/StreamPool.h>

using namespace net;
using namespace stream;

// Tests StreamPool add and next methods
TEST(StreamPoolTest, AddAndNextWork) { 
    StreamPool pool; 
    int size = 10, wvalue = 743, rvalue;   
    UINT8 data[size];

    NetHelper::writeUInt32(wvalue, data, 0);
    EXPECT_EQ(pool.getSize(0), 0);
    EXPECT_EQ(pool.getData(0), nullptr);

    pool.add(data, size);
    rvalue = (int)NetHelper::readUInt32(pool.getData(0), 0);
    EXPECT_EQ(pool.getSize(0), size);
    EXPECT_EQ(rvalue, wvalue);

    EXPECT_EQ(pool.next(), 0); 
    //make sure next does not change the readIndex
    rvalue = (int)NetHelper::readUInt32(pool.getData(0), 0);
    EXPECT_EQ(pool.getSize(0), size);
    EXPECT_EQ(rvalue, wvalue);
}