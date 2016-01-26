#include "common.h"
#include "burst.h"
#include "misc.h"
//一级cache
#define LEVEL L1
#define COL_WIDTH 6//块内偏移，64B
#define LINE_WIDTH 3//块选，８路
#define SET_WIDTH 7//组选,128组
#define FLAG_WIDTH (27 - COL_WIDTH - SET_WIDTH)
#define WRITE_THOUGH//写穿
#define rand(x) (x+1234)//定义的一种随机存取方式
#include "cache.h"
//二级cache
#define LEVEL L2
#define COL_WIDTH 6
#define LINE_WIDTH 4//16路
#define SET_WIDTH 12//4K个组
#define FLAG_WIDTH (27 - COL_WIDTH - SET_WIDTH)
#define WRITE_BACK//定义写回
#define WRITE_ALLOCATE
#define rand(x) ((x+1111)%100)
#include "cache.h"

void init_cache() {
	init_cache_L1();
	init_cache_L2();
}

uint32_t cache_read(hwaddr_t addr, size_t len) {
	return cache_read_L1(addr, len);
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	cache_write_L1(addr, len, data);
}

