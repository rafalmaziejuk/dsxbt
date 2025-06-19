idf_component_register(
    SRCS
    "ds4.cpp"
    "main.cpp"

    INCLUDE_DIRS 
    "."

    REQUIRES
    bluetooth
    nvs_flash
)
