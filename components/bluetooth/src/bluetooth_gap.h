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

class BluetoothGap final {
  public:
    [[nodiscard]] static Result initialize(const BluetoothGapConfig &config, BluetoothEventCallback eventCallback);

    static Result startDiscovery(esp_bt_inq_mode_t mode, uint8_t duration, uint8_t responsesCount);
    static Result stopDiscovery();

    static Result startRemoteServicesDiscovery(BluetoothDeviceAddress &address);
    static Result startRemoteServiceRecordDiscovery(BluetoothDeviceAddress &address, esp_bt_uuid_t uuid);
};

} // namespace dsx
