cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(gpio)

target_sources(app PRIVATE src/main.c src/controller.c src/heartbeat.c)
