idf_component_register(
    SRCS
    "src/bluetooth_gap.cpp"
    "src/bluetooth_service.cpp"
    
    INCLUDE_DIRS
    "include"
    "src"

    REQUIRES
    bt
    utils
)
