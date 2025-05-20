idf_component_register(
    SRCS
    "main.cpp"
    "application/application.cpp"

    INCLUDE_DIRS 
    "."

    REQUIRES
    bluetooth
    nvs_flash
    utils
)
