# CYB-3053 Project 2 - Memory Allocator

## Overview
This project implements a memory allocator in C, handling memory requests with `malloc()`, `calloc()`, `realloc()`, and `free()`. It tracks free memory blocks and requests more memory when needed.

## Features
- Implements First Fit allocation (Best Fit, Worst Fit, and Next Fit can be added)
- Uses a free list to track memory
- Splitting and coalescing for efficient memory usage

## Installation
```sh
git clone https://github.com/YOUR_USERNAME/OSProject2.git
cd OSProject2
mkdir build && cd build
cmake ..
make
./cyb3053_project2
