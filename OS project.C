#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
 
/* constants definitions*/
#define PAGES 256
#define PAGE_MASK 255
#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMO_SIZE PAGES * PAGE_SIZE
#define BUF_SIZE 10
 
/* This represents the physical page number for logical page*/
int pagetable[PAGES];
 
signed char main_Memo[MEMO_SIZE];
signed char *backing_ptr;
 
int main(int argc, const char *argv[])
{
    /check if the user entered 3 arguments otherwise exit/
    if (argc != 3) {
       printf( "Please enter 3 args: <./file_exe_name> <backing strore> <input file>\n");
        exit(0);
    }
     
    /* This will fill all the page table with -1*/
    for (int i = 0; i < PAGES; i++) { pagetable[i] = -1; } 
/* This is used to keep track and used to estimate statistics at end*/
       int total_addr = 0,pageFault = 0; 
/This file is the bin file(BACKING_STORE.bin)/
        const char *file_name = argv[1]; 
/This file is the input file(addresses.txt)/
        const char *input_file = argv[2];
        const char *output_file = "output.txt";
 /open and return a file descripter for the bin file/
        int backing_ptr_fd = open(file_name, O_RDONLY);
        backing_ptr = mmap(0, MEMO_SIZE, PROT_READ, MAP_PRIVATE, backing_ptr_fd, 0);
        FILE *input_fp = fopen(input_file, "r");
        FILE *output_fp=fopen(output_file, "w");
 /* This is used to read lines of input file*/
        char buf[BUF_SIZE];
 /* This is used to represent the number of the next unallocated physical page inside the main memory so we can use it*/
        unsigned char freePage = 0;
        while (fgets(buf, BUF_SIZE, input_fp) != NULL)
        { 
            int logical_addr = atoi(buf);
            int offset = logical_addr & OFFSET_MASK;
            int logical = (logical_addr >> OFFSET_BITS) & PAGE_MASK;
            int physical = pagetable[logical];
            total_addr++;    
        /* This if statement is dealing with page fault*/
        if (physical == -1) {
            pageFault++;
             
            physical = freePage;
            freePage++;
             
            /* read in a 256-byte page from the file BACKING_STORE and store it in an available page frame in physical memory*/
            memcpy(main_Memo + physical * PAGE_SIZE, backing_ptr + logical * PAGE_SIZE, PAGE_SIZE);
             
            pagetable[logical] = physical;
        }
         
         /The signed byte value stored at the translated physicall address/         
        int physicall_addr = (physical << OFFSET_BITS) | offset;
        signed char value = main_Memo[physical * PAGE_SIZE + offset];
         
        fprintf(output_fp,"Logical address: %d  physicall address:  %d  Value:  %d\n", logical_addr, physicall_addr, value);
    }
     
    printf("Number of Translated Addresses = %d\n", total_addr);
    printf("Page faults = %d\n", pageFault);
    printf("Page Fault Rate = %.1f %\n", (pageFault / (total_addr*1.))*100);
     
    return 0;
}