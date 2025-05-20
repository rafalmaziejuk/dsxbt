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

#include <utils/result.h>

#include <esp_bt.h>

namespace dsx {

struct BluetoothServiceConfig {
    esp_bt_mode_t mode;
};

class BluetoothService final {
  public:
    BluetoothService();
    ~BluetoothService();

    [[nodiscard]] Result initialize(const BluetoothServiceConfig &config);

  private:
    Result initializeBluetoothStack();

  private:
    struct Impl;
    Impl *m_impl{nullptr};
};

} // namespace dsx
