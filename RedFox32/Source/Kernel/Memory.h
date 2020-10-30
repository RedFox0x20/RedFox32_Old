#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

void *malloc(unsigned int Sz);
void *calloc(unsigned int Count, unsigned int Size);
void free(void *Memory);

#endif
