#include<stdio.h>
// create a structure because we want get both valid bit and tag
struct cache_line
{
    int valid_bit;
    unsigned int tag;
};
 struct cache_line my_cache[512];

 int access_cache(unsigned int address){
    unsigned int offset_bits=6;
    unsigned int index_bits=9;
    unsigned int tag_bits=32-index_bits-offset_bits;

    unsigned int index_mask=(1<<index_bits)-1;
    unsigned int tag_mask=(1<<tag_bits)-1;

    unsigned int index=(address>>offset_bits)&index_mask;
    unsigned int tag=(address>>offset_bits+index_bits)&tag_mask;
    printf("\nChecking Address: 0x%08x | Index: %d | Tag: 0x%x\n", address, index, tag);

    //logic to check hit or miss
    if(my_cache[index].valid_bit==1 && my_cache[index].tag==tag){
        printf("Result: HIT! \n");
        return 1;
    }else{
        printf("Result: MISS (installing new data)\n");
        my_cache[index].valid_bit=1;
        my_cache[index].tag=tag;
        return 0;
    }


}

int main(){
   unsigned int input_address=0x12345678;
   int total_access=0;
   int hit=0;

    for (int i = 0; i < 512; i++)
    {
       my_cache[i].valid_bit=0;
    }
    
    printf("--- CACHE SIMULATOR READY ---\n");
    printf("Type an address in Hex (e.g., 12345678) and press Enter.\n");
    printf("Type '0' to exit.\n\n");
    while (1) {
        printf("\n----------------------------------------\n");
        printf("Enter address: 0x");
        scanf("%x", &input_address);

        if (input_address == 0) {
            // Final Report
            printf("\n--- FINAL REPORT ---\n");
            printf("Total Accesses: %d\n", total_access);
            printf("Total Hits:     %d\n", hit);
            if (total_access > 0) {
                float rate = (float)hit / total_access * 100.0;
                printf("Hit Rate:       %.2f%%\n", rate);
            }
            break;
        }

        // Run logic and capture the result (1 or 0)
        int result = access_cache(input_address);
        
        // Update Stats
        total_access++;
        if (result == 1) {
            hit++;
        }

        // Live Report
        printf("Current Hit Rate: %.2f%% (%d/%d)\n", 
               (float)hit / total_access * 100.0, hit,total_access);
    }
    return 0;
}