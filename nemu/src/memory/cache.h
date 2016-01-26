typedef union {
	struct {
		uint32_t col	: COL_WIDTH;
		uint32_t set	: SET_WIDTH;
		uint32_t flag	: FLAG_WIDTH;
	};
	uint32_t addr;
} concat(cache_addr_, LEVEL);

#define NR_COL (1 << COL_WIDTH)
#define NR_LINE (1 << LINE_WIDTH)
#define NR_SET (1 << SET_WIDTH)
#define NR_FLAG (1 << FLAG_WIDTH)

#define HW_MEM_SIZE (1 << 27)

typedef struct {
	struct {
		uint8_t data[NR_LINE][NR_COL];
		bool valid[NR_LINE];
		bool dirty[NR_LINE];
		uint32_t flag[NR_LINE];
	} set[NR_SET];
} concat(cache_num_, LEVEL);

#define cache concat(cache_, LEVEL)
#define cache_addr concat(cache_addr_, LEVEL)
#define cache_num concat(cache_num_, LEVEL)
#define cache_read concat(cache_read_, LEVEL)
#define cache_write concat(cache_write_, LEVEL)
#define ccs3_read concat(ccs3_read_, LEVEL)
#define ccs3_write concat(ccs3_write_, LEVEL)
#define write_cache concat(write_cache_, LEVEL)
#define write_back concat(write_back_, LEVEL)


cache_num cache;

#if LEVEL ==L2
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
#else 
uint32_t concat(cache_read_, L1)(hwaddr_t, size_t);
void concat(cache_write_, L1)(hwaddr_t, size_t, uint32_t);
void concat(init_cache_, L1)();
#endif

void concat(init_cache_, LEVEL)() {
	int i,j;
	for(i=0;i<NR_SET;i++)
		for(j=0;j<NR_LINE;j++)
			cache.set[i].valid[j]=false;//将vaild位置为无效即可
#if LEVEL != L2
	concat(init_cache_, L1)();
#endif
}

#ifdef WRITE_BACK
void write_back(uint32_t set, uint32_t line, hwaddr_t addr) {
	if(!cache.set[set].dirty[line]) return;
	uint32_t col;
	for(col = 0; col < NR_COL; ++ col) {
#if LEVEL == L2
    dram_write((addr & ~(NR_COL - 1)) + col, 1, cache.set[set].data[line][col]);
#else 
    concat(cache_write_, L1)((addr & ~(NR_COL - 1)) + col, 1, cache.set[set].data[line][col]);
#endif
	}

}
#endif

void write_cache(uint32_t set, uint32_t line, uint32_t flag, uint32_t addr) {
	cache.set[set].valid[line] = true;
	cache.set[set].dirty[line] = false;
	cache.set[set].flag[line] = flag;
	uint32_t col;
	for(col = 0; col < NR_COL; ++ col) {
#if LEVEL == L2
	cache.set[set].data[line][col] = dram_read((addr & ~(NR_COL - 1)) + col, 1);
#else
	cache.set[set].data[line][col] = concat(cache_read_, L1)((addr & ~(NR_COL - 1)) + col, 1);
#endif
	}
}

void ccs3_read(hwaddr_t addr, void *data) {//仿照dram.c中的ddr3_read()
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	cache_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t col = temp.col;
	uint32_t set = temp.set;
	uint32_t flag = temp.flag;
	uint32_t line, line_ = 0;
	bool full = true, find = false;
	for(line = 0; line < NR_LINE; ++ line) {
		if(cache.set[set].valid[line]) {
			if(cache.set[set].flag[line] == flag) {
				memcpy(data, cache.set[set].data[line] + col, BURST_LEN);
				find = true;
				break;
			}
		} else {
			full = false;
			line_ = line;
		}
	}
	if(!find) {
		if(full) {
			line_ = rand(addr) & (NR_LINE - 1);
#ifdef WRITE_BACK
			write_back(set, line_, addr);
#endif
		}
		write_cache(set, line_, flag, addr);
		memcpy(data, cache.set[set].data[line_] + col, BURST_LEN);
	}
}

void ccs3_write(hwaddr_t addr, void *data, uint8_t *mask) {//仿照dram.c中的ddr3_write()
	Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	cache_addr temp;
	temp.addr = addr & ~BURST_MASK;
	uint32_t col = temp.col;
	uint32_t set = temp.set;
	uint32_t flag = temp.flag;
	uint32_t line;
#ifdef WRITE_ALLOCATE	
	bool full = true, find = false;
	uint32_t line_ = 0;
#endif
	
	for(line = 0; line < NR_LINE; ++ line) {
		if(cache.set[set].valid[line]) {
			if(cache.set[set].flag[line] == flag) {
#ifdef WRITE_ALLOCATE	
			find = true;
#endif
			cache.set[set].dirty[line] = true;
			memcpy_with_mask(cache.set[set].data[line] + col, data, BURST_LEN, mask);
				break;
			}
		} else {
#ifdef WRITE_ALLOCATE	
			full = false;
			line_ = line;
#endif
		}
	}

#ifdef WRITE_ALLOCATE	
	if(!find) {
		if(full) {
			line_ = rand(addr) & (NR_LINE - 1);
#ifdef WRITE_BACK
			write_back(set, line_, addr);
#endif
		}
		write_cache(set, line_, flag, addr);
		memcpy_with_mask(cache.set[set].data[line_] + col, data, BURST_LEN, mask);
	}
#endif

}

uint32_t cache_read(hwaddr_t addr, size_t len) {//仿照dram.c中的dram_read()

	uint32_t offset = addr & BURST_MASK;
	uint8_t temp[2 * BURST_LEN];

	ccs3_read(addr, temp);

	if(offset + len > BURST_LEN) {
		ccs3_read(addr + BURST_LEN, temp + BURST_LEN);
	}
	
	return unalign_rw(temp + offset, 4);
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {//仿照dram.c中的dram_write()
	uint32_t offset = addr & BURST_MASK;
	uint8_t temp[2 * BURST_LEN];
	uint8_t mask[2 * BURST_LEN];
	memset(mask, 0, 2 * BURST_LEN);

	*(uint32_t *)(temp + offset) = data;
	memset(mask + offset, 1, len);

	ccs3_write(addr, temp, mask);

	if(offset + len > BURST_LEN) {
		ccs3_write(addr + BURST_LEN, temp + BURST_LEN, mask + BURST_LEN);
	}
#ifdef WRITE_THOUGH
	#if LEVEL == L2
		dram_write(addr, len, data);
	#else
		concat(cache_write_, L1)(addr, len, data);
	#endif
#endif
}


#undef COL_WIDTH
#undef LINE_WIDTH
#undef SET_WIDTH
#undef FLAG_WIDTH
#undef rand
#undef LEVEL

#undef NR_COL
#undef NR_LINE
#undef NR_SET
#undef NR_FLAG
#undef HW_MEM_SIZE

#undef cache_addr
#undef cache_num
#undef cache_read
#undef cache_write
#undef ccs3_read
#undef ccs3_write
#undef write_cache
#undef write_back

#ifdef WRITE_THOUGH
	#undef WRITE_THOUGH
#else 
	#undef WRITE_BACK
#endif

