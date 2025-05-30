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
#include <bluetooth/bluetooth_service.h>

#include <optional>

namespace dsx {

class BluetoothEvent;
struct BluetoothDeviceDiscoveredEvent;
struct BluetoothRemoteServiceRecordDiscoveredEvent;

class Application final {
  public:
    Application();
    ~Application();

    void run();

  private:
    void onBluetoothEvent(BluetoothEvent &event);
    void onBluetoothDeviceDiscoveredEvent(const BluetoothDeviceDiscoveredEvent &event);
    void onRemoteServiceRecordDiscoveredCallback(const BluetoothRemoteServiceRecordDiscoveredEvent &event);

  private:
    BluetoothService m_bluetoothService{};
    std::optional<BluetoothDevice> m_bluetoothDevice{};
};

} // namespace dsx
