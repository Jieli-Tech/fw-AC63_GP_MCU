#ifndef BSP_MALLOC_H
#define BSP_MALLOC_H

extern void memory_init();

extern void *malloc(unsigned long size);

extern void *zalloc(unsigned long size);

extern void free(void *ptr);


#endif

