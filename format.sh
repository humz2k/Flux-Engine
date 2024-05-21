#!/usr/bin/env bash

clang-format -i src/**/*.c
clang-format -i src/**/*.h
clang-format -i project/scripts/*.c
clang-format -i project/main.c
clang-format -i drivers/*.c