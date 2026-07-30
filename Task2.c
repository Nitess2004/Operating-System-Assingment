

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 

 #define PAGE_SIZE        256      
 #define VIRTUAL_MEM_SIZE 4096     
 #define NUM_PAGES        (VIRTUAL_MEM_SIZE / PAGE_SIZE)  
 #define FRAMES           4       
 #define TOTAL_REFS       12       
 
 
 int virtual_addresses[TOTAL_REFS] = {
     9,8,1,4,9,6,7,6,6,4,1
 };
 

 int address_to_page(int addr) {
     return addr / PAGE_SIZE;
 }
 

 void build_reference_string(int pages[]) {
     for (int i = 0; i < TOTAL_REFS; i++)
         pages[i] = address_to_page(virtual_addresses[i]);
 }
 
 void print_frame_state(int frame[], int valid[]) {
     for (int j = 0; j < FRAMES; j++) {
         if (valid[j])
             printf("P%-4d ", frame[j]);
         else
             printf("-     ");
     }
     printf("\n");
 }
 

 void FIFO(int pages[]) {
     int frame[FRAMES];
     int valid[FRAMES] = {0};
     int hits = 0, faults = 0;
     int next_victim = 0; 
 
     printf("\n========== FIFO Page Replacement ==========\n");
     printf("Page Size: %d bytes | Frames: %d | Pages in address space: %d\n\n",
            PAGE_SIZE, FRAMES, NUM_PAGES);
 
     for (int i = 0; i < TOTAL_REFS; i++) {
         int page = pages[i];
         int found = 0;
 
         for (int j = 0; j < FRAMES; j++) {
             if (valid[j] && frame[j] == page) {
                 found = 1;
                 hits++;
                 break;
             }
         }
 
         if (!found) {
             frame[next_victim] = page;
             valid[next_victim] = 1;
             next_victim = (next_victim + 1) % FRAMES;
             faults++;
         }
 
         printf("Addr %4d -> Page %-3d [%s] : ",
                virtual_addresses[i], page, found ? "HIT " : "FAULT");
         print_frame_state(frame, valid);
     }
 
     printf("\nHits        : %d\n", hits);
     printf("Page Faults : %d\n", faults);
     printf("Hit Ratio   : %.2f\n", (float)hits / TOTAL_REFS);
     printf("Miss Ratio  : %.2f\n", (float)faults / TOTAL_REFS);
 }
 

 void LRU(int pages[]) {
     int frame[FRAMES];
     int valid[FRAMES] = {0};
     int last_used[FRAMES];
     int hits = 0, faults = 0;
 
     for (int j = 0; j < FRAMES; j++)
         last_used[j] = -1;
 
     printf("\n========== LRU Page Replacement ==========\n");
     printf("Page Size: %d bytes | Frames: %d | Pages in address space: %d\n\n",
            PAGE_SIZE, FRAMES, NUM_PAGES);
 
     for (int i = 0; i < TOTAL_REFS; i++) {
         int page = pages[i];
         int found = 0;
 
         for (int j = 0; j < FRAMES; j++) {
             if (valid[j] && frame[j] == page) {
                 found = 1;
                 hits++;
                 last_used[j] = i; 
                 break;
             }
         }
 
         if (!found) {
            
             int victim = 0;
             int empty_found = 0;
             for (int j = 0; j < FRAMES; j++) {
                 if (!valid[j]) { victim = j; empty_found = 1; break; }
             }
             if (!empty_found) {
                 victim = 0;
                 for (int j = 1; j < FRAMES; j++) {
                     if (last_used[j] < last_used[victim])
                         victim = j;
                 }
             }
             frame[victim] = page;
             valid[victim] = 1;
             last_used[victim] = i;
             faults++;
         }
 
         printf("Addr %4d -> Page %-3d [%s] : ",
                virtual_addresses[i], page, found ? "HIT " : "FAULT");
         print_frame_state(frame, valid);
     }
 
     printf("\nHits        : %d\n", hits);
     printf("Page Faults : %d\n", faults);
     printf("Hit Ratio   : %.2f\n", (float)hits / TOTAL_REFS);
     printf("Miss Ratio  : %.2f\n", (float)faults / TOTAL_REFS);
 }
 
 int main(void) {
     int pages[TOTAL_REFS];
     build_reference_string(pages);
 
     printf("========================================\n");
     printf("   VIRTUAL MEMORY PAGING SIMULATION\n");
     printf("========================================\n");
     printf("Virtual address space : %d bytes\n", VIRTUAL_MEM_SIZE);
     printf("Page size             : %d bytes\n", PAGE_SIZE);
     printf("Number of pages       : %d\n", NUM_PAGES);
     printf("Physical frames       : %d\n\n", FRAMES);
 
     printf("Virtual address reference stream:\n");
     for (int i = 0; i < TOTAL_REFS; i++)
         printf("%d ", virtual_addresses[i]);
     printf("\n\nCorresponding page reference string:\n");
     for (int i = 0; i < TOTAL_REFS; i++)
         printf("P%d ", pages[i]);
     printf("\n");
 
     FIFO(pages);
     LRU(pages);
 
     printf("\n========== Summary Comparison ==========\n");
     printf("(see individual results above; both algorithms ran on the\n");
     printf(" identical page reference string derived from the same\n");
     printf(" virtual address stream, so their hit/miss ratios are\n");
     printf(" directly comparable)\n");
 
     return 0;
 }
