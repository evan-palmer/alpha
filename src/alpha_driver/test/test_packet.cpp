// Copyright 2023, Evan Palmer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <gmock/gmock.h>

#include "alpha_driver/crc.hpp"
#include "alpha_driver/packet.hpp"
#include "alpha_driver/packet_id.hpp"

namespace test_alpha_driver
{

TEST(PacketTest, TestPacketEncode)
{
  const std::vector<unsigned char> data = {0x01, 0x02, 0x03, 0x04};

  // Create an encoded test packet using the BPL structure based off of the test
  // data
  const std::vector<unsigned char> expected_encoding = {0x09, 0x01, 0x02, 0x03, 0x04,
                                                        0x01, 0xFF, 0x08, 0x5D, 0x00};

  // Construct a new packet using the data and the expected IDs
  auto packet =
    alpha_driver::Packet(alpha_driver::PacketId::kMode, alpha_driver::DeviceId::kAllJoints, data);

  ASSERT_THAT(packet.Encode(), ::testing::ElementsAreArray(expected_encoding));
}

TEST(PacketTest, TestPacketDecode)
{
  const std::vector<unsigned char> encoded_data = {0x09, 0x01, 0x02, 0x03, 0x04,
                                                   0x01, 0xFF, 0x08, 0x5D, 0x00};

  const std::vector<unsigned char> decoded_data = {0x01, 0x02, 0x03, 0x04};

  const alpha_driver::Packet packet = alpha_driver::Packet::Decode(encoded_data);

  ASSERT_THAT(packet.data(), ::testing::ElementsAreArray(decoded_data));
}

}  // namespace test_alpha_driver

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  const int result = RUN_ALL_TESTS();

  return result;
}
