idf_component_register(
    SRCS
    "src/gpio_manager.cpp"
    
    INCLUDE_DIRS
    "include"
    "src"

    REQUIRES
    esp_driver_gpio
    utils
    threading
)
