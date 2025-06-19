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

class BluetoothHidHost final {
  public:
    static Result initialize(BluetoothEventCallback eventCallback);
    static Result deinitialize();

    static Result openHidDeviceConnection(BluetoothDeviceAddress &address, esp_hid_transport_t transport, esp_ble_addr_type_t bleAddressType);
    static Result getHidDeviceFeatureReport(esp_hidh_dev_t *deviceData, size_t reportMapIndex, size_t reportId, size_t size, uint8_t *buffer, size_t *sizeOut);
    static Result getHidDeviceBluetoothAddress(esp_hidh_dev_t *deviceData, BluetoothDeviceAddress &addressOut);
};

} // namespace dsx
