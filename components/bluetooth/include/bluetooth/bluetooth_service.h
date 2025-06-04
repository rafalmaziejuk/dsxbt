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

class BluetoothDevice;

class BluetoothService final {
  public:
    BluetoothService();
    ~BluetoothService();

    [[nodiscard]] Result initialize(const BluetoothServiceConfig &config);
    [[nodiscard]] Result initializeHidHost();

    [[nodiscard]] Result startDiscovery(esp_bt_inq_mode_t mode, uint8_t duration = 10u, uint8_t responsesCount = 0u);
    [[nodiscard]] Result stopDiscovery();

    [[nodiscard]] Result startRemoteServicesDiscovery(const BluetoothDevice &device);
    [[nodiscard]] Result startRemoteServiceRecordDiscovery(const BluetoothDevice &device, esp_bt_uuid_t uuid);

    [[nodiscard]] Result openHidDeviceConnection(const BluetoothDevice &device, esp_hid_transport_t transport, esp_ble_addr_type_t bleAddressType);

  private:
    struct Impl;
    Impl *m_impl{nullptr};
};

} // namespace dsx
