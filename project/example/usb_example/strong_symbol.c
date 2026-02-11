/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"


u8 mem_heap[8*1024] AT(.mem_heap);

static const struct thread_cfg_cb thread_cfg =
{
   .bt_hw      =       {0, 6,  1024},
   .bt_stack   =       {0, 23, 1536},
   .aupcm      =       {1, 17, 1024},
   .dirver     =       {1, 11, 1024},
   .timer      =       {1, 16, 768},
   .main       =       {1, 25, 1024},
};

uint32_t mem_heap_size_get(void)
{
   return sizeof(mem_heap);
}

struct thread_cfg_cb* thread_cfg_get(void)
{
   return (struct thread_cfg_cb*)&thread_cfg;
}