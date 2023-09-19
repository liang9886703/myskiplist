#ifndef ToolFunctions_H
#define ToolFunctions_H

#ifdef TEST_DEBUG
#define DEBUG_PRINT_LINE(...) do{printf("[Info] %s::%d\t ", __FILE__, __LINE__); printf( __VA_ARGS__);printf("\n");}while(0)
//#define DEBUG_PRINT_KV_LINE(key, value, fun1, fun2, level) \
//do{printf("[Info] %s::%d\t ", __FILE__, __LINE__); \
//	printf("Key is"); fun1(key); \
//	printf("Value is"); fun2(value);\
//	printf("level is %d", level);\
//	printf("\n") \
//;}while(0)

#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#define ERROR_PRINT_LINE(...)  do{printf("[Error] %s::%d\t ", __FILE__, __LINE__); printf( __VA_ARGS__);printf("\n");}while(0)
#else
#define DEBUG_PRINT_LINE(...)
#define DEBUG_PRINT(...)
#define ERROR_PRINT_LINE(...)
#endif

#endif