// Copyright 2025 Rafal Maziejuk
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <bluetooth/bluetooth_manager.h>

namespace dsx {

enum class BluetoothDeviceState : uint8_t {
    k_unknown,
    k_discovered,
    k_connected
};

class BluetoothDevice {
  public:
    [[nodiscard]] BluetoothDeviceState getState() const;

  public:
    BluetoothDevice(const BluetoothDevice &) = delete;
    BluetoothDevice &operator=(const BluetoothDevice &) = delete;
    BluetoothDevice(BluetoothDevice &&) = delete;
    BluetoothDevice &operator=(BluetoothDevice &&) = delete;
    virtual ~BluetoothDevice() = default;

  protected:
    explicit BluetoothDevice(BluetoothManager &bluetoothManager);

    virtual void onEvent(BluetoothEvent &bluetoothEvent) = 0;

  protected:
    BluetoothDeviceState m_state{BluetoothDeviceState::k_unknown};
    BluetoothDeviceAddress m_address{};

    BluetoothManager &m_rBluetoothManager;
};

} // namespace dsx
