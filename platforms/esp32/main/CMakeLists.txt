idf_component_register(
  SRCS "main.cpp"
  INCLUDE_DIRS "."
  REQUIRES edgetx nvs_flash esp_timer driver
)
