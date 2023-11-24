#pragma once

#ifdef NDEBUG
#define LOG(msg) (void)0
#else 
#define LOG(msg, ...) printf(msg, __VA_ARGS__), printf("\n")
#endif