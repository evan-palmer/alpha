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

#include "alpha_hardware/hardware.hpp"

#include <chrono>
#include <limits>

#include "alpha_driver/packet_id.hpp"

namespace alpha_hardware
{

hardware_interface::CallbackReturn AlphaHardware::on_init(
  const hardware_interface::HardwareInfo & info)
{
  if (
    hardware_interface::SystemInterface::on_init(info) !=
    hardware_interface::CallbackReturn::SUCCESS) {
    return hardware_interface::CallbackReturn::ERROR;
  }

  hw_states_position_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
  hw_states_velocity_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
  hw_commands_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());

  // Load ROS params
  serial_port_ = std::stod(info_.hardware_parameters["serial_port"]);
  heartbeat_timeout_ = std::stod(info_.hardware_parameters["heartbeat_timeout"]);
  state_update_freq_ = std::stod(info_.hardware_parameters["state_update_frequency"]);

  // Start the driver
  if (!driver_.start(serial_port_, heartbeat_timeout_)) {
    return hardware_interface::CallbackReturn::ERROR;
  }

  // Register callbacks for joint states
  driver_.subscribe(
    alpha_driver::PacketId::kPosition,
    std::bind(&AlphaHardware::update_position_cb, this, std::placeholders::_1));

  driver_.subscribe(
    alpha_driver::PacketId::kVelocity,
    std::bind(&AlphaHardware::update_velocity_cb, this, std::placeholders::_1));

  // Start a thread to request state updates
  running_.store(true);
  state_request_worker_ = std::thread(&AlphaHardware::poll_state, this, state_update_freq_);

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn AlphaHardware::on_cleanup(const rclcpp_lifecycle::State &)
{
  running_.store(false);
  state_request_worker_.join();
  driver_.stop();
}

std::vector<hardware_interface::StateInterface> AlphaHardware::export_state_interfaces()
{ /* data */
}

std::vector<hardware_interface::CommandInterface> AlphaHardware::export_command_interfaces()
{ /* data */
}

hardware_interface::CallbackReturn AlphaHardware::on_activate(
  const rclcpp_lifecycle::State & previous_state)
{
  /* data */
}

hardware_interface::CallbackReturn AlphaHardware::on_deactivate(
  const rclcpp_lifecycle::State & previous_state)
{
  /* data */
}

hardware_interface::return_type AlphaHardware::read(
  const rclcpp::Time & time, const rclcpp::Duration & period)
{
  /* data */
}

hardware_interface::return_type AlphaHardware::write(
  const rclcpp::Time & time, const rclcpp::Duration & period)
{
  /* data */
}

void AlphaHardware::update_position_cb(const alpha_driver::Packet & packet)
{ /* data */
}

void AlphaHardware::update_velocity_cb(const alpha_driver::Packet & packet)
{ /* data */
}

void AlphaHardware::poll_state(const int freq) const
{
  // Request position and velocity state information
  std::vector<alpha_driver::PacketId> packets = {
    alpha_driver::PacketId::kPosition, alpha_driver::PacketId::kVelocity};

  while (running_.load()) {
    driver_.request(packets, alpha_driver::DeviceId::kAllJoints);
    std::this_thread::sleep_for(std::chrono::seconds(1 / freq));
  }
}

}  // namespace alpha_hardware
