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

#include <bluetooth/bluetooth_types.h>
#include <utils/result.h>

namespace dsx {

class BluetoothManager final {
  public:
    BluetoothManager() = default;
    BluetoothManager(const BluetoothManager &) = delete;
    BluetoothManager &operator=(const BluetoothManager &) = delete;
    BluetoothManager(BluetoothManager &&) = delete;
    BluetoothManager &operator=(BluetoothManager &&) = delete;
    ~BluetoothManager();

    [[nodiscard]] Result initialize(const BluetoothManagerConfig &config);

    // GAP API
    [[nodiscard]] Result startDiscovery(esp_bt_inq_mode_t mode, uint8_t duration = 10u, uint8_t responsesCount = 0u);
    [[nodiscard]] Result stopDiscovery();
    [[nodiscard]] Result startRemoteServicesDiscovery(BluetoothDeviceAddress &address);
    [[nodiscard]] Result startRemoteServiceRecordDiscovery(BluetoothDeviceAddress &address, esp_bt_uuid_t uuid);

    // HID Host API
    [[nodiscard]] Result initializeHidHost(const BluetoothHidHostConfig &config);
    [[nodiscard]] Result openHidDeviceConnection(BluetoothDeviceAddress &address, esp_hid_transport_t transport, esp_ble_addr_type_t bleAddressType);
    [[nodiscard]] Result getHidDeviceFeatureReport(esp_hidh_dev_t *deviceData, size_t reportMapIndex, size_t reportId, size_t size, uint8_t *buffer, size_t *sizeOut);

    [[nodiscard]] Result getHidDeviceBluetoothAddress(esp_hidh_dev_t *deviceData, BluetoothDeviceAddress &addressOut);

  private:
    Result initializeBluetoothStack(const BluetoothManagerConfig &config);

  private:
    bool m_isInitialized{false};
    bool m_isHidHostInitialized{false};
};

} // namespace dsx
