cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr)
project(nexysa7)

target_sources(app PRIVATE src/main.c ../../common/heartbeat.c ../../common/sw0.c)
