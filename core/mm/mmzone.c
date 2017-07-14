#include "mmzone.h"
#include "mm.h"
#include "coalition_allocator.h"

extern void* get_fragment_page();
extern void free_fragment_page(uint32_t page_addr);
extern void fragment_allocator_init(uint32_t start_addr,uint32_t size);

void mm_zone_init(uint32_t addr,size_t size)
{
    //we give 24M for high memory
    //the other used for normal memory
    //normal memory
    printf("zone init size is %d \n",size);
    zone_list[ZONE_NORMAL].start_pa = addr;
    zone_list[ZONE_NORMAL].end_pa = addr + size - ZONE_HIGH_MEMORY;
    zone_list[ZONE_NORMAL].alloctor_init = coalition_allocator_init;
    zone_list[ZONE_NORMAL].alloctor_get_memory = coalition_malloc;
    zone_list[ZONE_NORMAL].alloctor_free = coalition_free;
    zone_list[ZONE_NORMAL].alloctor_pmem = pmem_malloc;
    zone_list[ZONE_NORMAL].alloctor_pmem_free = pmem_free;
    zone_list[ZONE_NORMAL].alloctor_init(addr,size - ZONE_HIGH_MEMORY);

    //high memory,high memory's memory alloctor need rewrite....
    zone_list[ZONE_HIGH].start_pa = zone_list[ZONE_NORMAL].end_pa + PAGE_SIZE;
    zone_list[ZONE_HIGH].end_pa = addr + size;
    zone_list[ZONE_HIGH].alloctor_init = fragment_allocator_init;
    zone_list[ZONE_HIGH].alloctor_get_memory = get_fragment_page;
    zone_list[ZONE_HIGH].alloctor_free = free_fragment_page;
    zone_list[ZONE_HIGH].alloctor_init(zone_list[ZONE_HIGH].start_pa,size - zone_list[ZONE_HIGH].start_pa);
}

void *zone_get_page(int type,uint32_t size)
{
    return zone_list[type].alloctor_get_memory(size);
}

void zone_free_page(int type,addr_t ptr)
{
    zone_list[type].alloctor_free(ptr);
}

void *zone_get_pmem(size)
{
    return zone_list[ZONE_NORMAL].alloctor_pmem(size);
}




