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

#include <gpio/gpio_manager.h>

#include <esp_attr.h>

DSX_LOG_TAG(GpioManager);

namespace dsx {

Result GpioManager::installInterruptService(int32_t interruptAllocationFlags) {
    esp_err_t error = gpio_install_isr_service(interruptAllocationFlags);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to install GPIO interrupt service");
    }

    return DSX_RESULT_SUCCESS();
}

Result GpioManager::configureGpio(gpio_config_t config) {
    esp_err_t error = gpio_config(&config);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to configure GPIO");
    }

    return DSX_RESULT_SUCCESS();
}

Result GpioManager::addInterruptHandler(const std::vector<gpio_num_t> &gpios, GpioInterruptHandler handler) {
    constexpr size_t k_messageQueueLength = 10u;

    auto queue = std::make_shared<MessageQueue<gpio_num_t>>(handler, k_messageQueueLength);
    auto result = queue->start();
    if (result == ESP_FAIL) {
        return result;
    }

    for (const auto gpio : gpios) {
        auto context = std::make_unique<GpioIsrHandlerContext>(gpio, queue);
        esp_err_t error = gpio_isr_handler_add(gpio, gpioIsrHandler, context.get());
        if (error != ESP_OK) {
            return DSX_RESULT_ERROR(error, "unable to add interrupt handler for GPIO{}", static_cast<int8_t>(gpio));
        }

        m_interruptContexts.push_back(std::move(context));
    }

    m_queues.push_back(std::move(queue));

    return DSX_RESULT_SUCCESS();
}

void IRAM_ATTR GpioManager::gpioIsrHandler(void *arg) {
    auto *context = reinterpret_cast<GpioIsrHandlerContext *>(arg);
    if (context && context->queue) {
        DSX_RESULT_CHECK(context->queue->postIsr(context->gpio));
    }
}

} // namespace dsx
