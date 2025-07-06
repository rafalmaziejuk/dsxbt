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

#include <bluetooth/bluetooth_device.h>
#include <gpio/gpio_manager.h>

namespace dsx {

struct BluetoothDeviceDiscoveredEvent;
struct BluetoothRemoteServiceRecordDiscoveredEvent;
struct BluetoothHidDeviceOpenEvent;
struct BluetoothHidDeviceCloseEvent;

class DualShock4 : public BluetoothDevice {
  public:
    explicit DualShock4(BluetoothManager &bluetoothManager,
                        GpioManager &gpioManager);
    ~DualShock4() override;

    [[nodiscard]] Result discover() const;

  private:
    void onEvent(BluetoothEvent &bluetoothEvent) override;
    void onDeviceDiscoveredEvent(const BluetoothDeviceDiscoveredEvent &event);
    void onRemoteServiceRecordDiscoveredEvent(const BluetoothRemoteServiceRecordDiscoveredEvent &event);
    void onHidDeviceOpenedEvent(const BluetoothHidDeviceOpenEvent &event);
    void onHidDeviceClosedEvent(const BluetoothHidDeviceCloseEvent &event);

  private:
    bool m_discovering{false};
    GpioManager &m_rGpioManager;
};

} // namespace dsx
