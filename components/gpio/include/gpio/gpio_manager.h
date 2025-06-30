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
#include <threading/message_queue.h>

#include <driver/gpio.h>

#include <memory>
#include <vector>

namespace dsx {

using GpioInterruptHandler = std::function<void(gpio_num_t)>;

class GpioManager final {
  public:
    [[nodiscard]] Result installInterruptService(int32_t interruptAllocationFlags = 0);
    [[nodiscard]] Result configureGpio(gpio_config_t config);
    [[nodiscard]] Result addInterruptHandler(const std::vector<gpio_num_t> &gpios, GpioInterruptHandler handler);

  public:
    GpioManager() = default;
    GpioManager(const GpioManager &) = delete;
    GpioManager &operator=(const GpioManager &) = delete;
    GpioManager(GpioManager &&) = delete;
    GpioManager &operator=(GpioManager &&) = delete;

  private:
    struct GpioIsrHandlerContext {
        gpio_num_t gpio;
        std::shared_ptr<MessageQueue<gpio_num_t>> queue{nullptr};
    };

    static void gpioIsrHandler(void *arg);

  private:
    std::vector<std::unique_ptr<GpioIsrHandlerContext>> m_interruptContexts;
    std::vector<std::shared_ptr<MessageQueue<gpio_num_t>>> m_queues;
};

} // namespace dsx
