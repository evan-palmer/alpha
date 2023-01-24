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

#pragma once

#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "alpha_driver/packet.hpp"
#include "alpha_driver/packet_id.hpp"

namespace alpha_driver
{

class SerialClient
{
public:
  /**
   * @brief Construct a new Serial Client object.
   *
   * @param device
   * @param timeout_ms
   * @param blocking
   */
  SerialClient(const std::string & device, const int timeout_ms = 500, const bool blocking = false);

  /**
   * @brief Destroy the Serial Client object. This shuts down all threads.
   *
   */
  ~SerialClient();

  /**
   * @brief Send a packet over the serial connection.
   *
   * @param packet message to send to the Reach Alpha manipulator; the data should not yet be
   * encoded.
   */
  auto Send(const Packet & packet) const -> void;

  /**
   * @brief Register a new callback function to a specified packet type.
   *
   * @param packet_type type of packet that the callback should be signaled on
   * @param callback function that should be executed when a message of a given type is received
   */
  auto Receive(const PacketId packet_type, const std::function<void(Packet)> & callback) -> void;

  /**
   * @brief Indicates whether or not the arm connection is currently active. To be considered
   * 'active' there must be an open serial connection and the client must be actively receiving
   * heartbeat packets from the Reach Alpha manipulator.
   *
   * @return true
   * @return false
   */
  auto active() const -> bool;

private:
  /**
   * @brief Current state of the serial client.
   *
   * @note This is used to manage the polling of the RX thread and helps us shutdown cleanly.
   *
   */
  enum class ClientState
  {
    kRunning,  // The serial client is running
    kStopped   // The serial client is stopped; signal thread destruction
  };

  /**
   * @brief Defines the current state of the serial port.
   *
   */
  enum class PortState
  {
    kOpen,   // The serial port is currently open
    kClosed  // The serial port is currently closed
  };

  /**
   * @brief Defines the current state of the Reach Alpha heartbeat.
   *
   */
  enum class HeartbeatState
  {
    kBeating,  // The arm is actively sending heartbeat messages
    kTimeout,  // The arm was beating but timed out
    kDead      // The arm heartbeat was never started
  };

  // Map used to store the callback functions for messages. Keys should be the ID for a message,
  // and the values are the callback functions to execute when a message is received.
  std::unordered_map<PacketId, std::vector<std::function<void(Packet)>>> callbacks_;

  // Serial port file descriptor
  int fd_;

  ClientState client_status_;  // Serial client is currently running or not; used to stop thread
  PortState port_status_;      // Status of the serial port; should be either open or closed
  HeartbeatState heartbeat_status_;  // Status of the arm heartbeat;

  // Thread responsible for receiving incoming data and executing the respective callbacks
  std::thread rx_worker_;

  // Incoming data buffer
  std::vector<unsigned char> buffer_;
};

}  // namespace alpha_driver
