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

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <functional>
#include <thread>

namespace dsx {

template <typename Message>
class MessageQueue {
    DSX_LOG_TAG(MessageQueue);
    using MessageHandler = std::function<void(const Message &)>;

  public:
    explicit MessageQueue(MessageHandler handler, size_t length)
        : m_handler{handler},
          m_length{length} {}

    ~MessageQueue() {
        DSX_RESULT_CHECK(stop());

        vQueueDelete(m_queue);
        m_queue = nullptr;
    }

    [[nodiscard]] Result start() {
        if (m_running) {
            DSX_LOGW("message queue already started");
            return DSX_RESULT_SUCCESS();
        }

        if (m_queue == nullptr) {
            m_queue = xQueueCreate(m_length, sizeof(InternalMessage));
            if (m_queue == nullptr) {
                return DSX_RESULT_ERROR(ESP_FAIL, "unable to create message queue");
            }
        }

        m_thread = std::thread(&MessageQueue::worker, this);
        m_running = true;

        return DSX_RESULT_SUCCESS();
    }

    [[nodiscard]] Result stop() {
        if (!m_running) {
            DSX_LOGW("message queue already stopped");
            return DSX_RESULT_SUCCESS();
        }

        InternalMessage internalMessage{
            .message = {},
            .stop = true,
        };
        xQueueSend(m_queue, &internalMessage, pdMS_TO_TICKS(500));

        if (m_thread.joinable()) {
            m_thread.join();
        }
        m_running = false;

        return DSX_RESULT_SUCCESS();
    }

    [[nodiscard]] Result post(const Message &message) {
        InternalMessage internalMessage{
            .message = message,
            .stop = false,
        };
        if (xQueueSend(m_queue, &internalMessage, pdMS_TO_TICKS(500)) != pdPASS) {
            return DSX_RESULT_ERROR(ESP_FAIL, "unable to post message");
        }

        return DSX_RESULT_SUCCESS();
    }

    [[nodiscard]] Result postIsr(const Message &message) {
        InternalMessage internalMessage{
            .message = message,
            .stop = false,
        };
        if (xQueueSendFromISR(m_queue, &internalMessage, nullptr) != pdPASS) {
            return DSX_RESULT_ERROR(ESP_FAIL, "unable to post message from ISR");
        }

        return DSX_RESULT_SUCCESS();
    }

  private:
    struct InternalMessage {
        Message message;
        bool stop;
    };

    void worker() {
        while (true) {
            InternalMessage internalMessage{};
            if (xQueueReceive(m_queue, &internalMessage, portMAX_DELAY) == pdPASS) {
                if (internalMessage.stop) {
                    return;
                }

                m_handler(internalMessage.message);
            }
        }
    }

  public:
    MessageQueue(const MessageQueue &) = delete;
    MessageQueue &operator=(const MessageQueue &) = delete;
    MessageQueue(MessageQueue &&) = delete;
    MessageQueue &operator=(MessageQueue &&) = delete;

  private:
    MessageHandler m_handler;
    std::thread m_thread;
    size_t m_length;
    QueueHandle_t m_queue{nullptr};
    bool m_running{false};
};

} // namespace dsx
