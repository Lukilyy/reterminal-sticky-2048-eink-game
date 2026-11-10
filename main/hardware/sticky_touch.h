#pragma once

#include <cstdint>
#include <atomic>

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "gt911.h"

enum class TouchEventType {
    None,
    Tap,
    SwipeUp,
    SwipeDown,
    SwipeLeft,
    SwipeRight,
};

struct TouchEvent {
    TouchEventType type = TouchEventType::None;
    uint16_t x = 0;
    uint16_t y = 0;
};

class StickyTouch {
public:
    bool init();
    // Finish any in-flight I2C read and stop polling before cutting touch power.
    void stop();
    TouchEvent poll();

private:
    struct Point {
        uint16_t x = 0;
        uint16_t y = 0;
    };

    static void touch_task_entry(void *context);
    void touch_task();
    TouchEvent read_touch();

    i2c_master_bus_handle_t i2c_bus_ = nullptr;
    GT911 controller_;
    QueueHandle_t event_queue_ = nullptr;
    bool task_started_ = false;
    std::atomic<bool> stop_requested_{false};
    std::atomic<bool> task_stopped_{false};
    bool touching_ = false;
    Point start_ = {};
    Point last_ = {};
};
