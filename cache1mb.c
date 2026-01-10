#include<stdio.h>
struct cache1_line
{
    int valid_bit;
    unsigned int tag;
    int last_access_time;

};
struct cache_set
{
   struct cache1_line lines[4];
};
struct cache_set my_cache[4096];


int access_cache(int address,int current_time){

    unsigned int index_bits=12;
    unsigned int offset_bits=6;
    unsigned int tag_bits=14;

    //create mask
    unsigned int offset_mask=(1<<offset_bits)-1;
    unsigned int index_mask=(1<<index_bits)-1;
    unsigned int tag_mask=(1<<tag_bits)-1;

    unsigned int offset=address&offset_mask;
    unsigned int index=(address>>offset_bits)&index_mask;
    unsigned int tag=(address>>(offset_bits+index_bits))&tag_mask;

    printf("\nchecking address: 0x%08x  |Tag : %d  | index : 0x%x\n",address,tag,index);

    for(int i =0;i<4;i++){
        if(my_cache[index].lines[i].valid_bit==1 && my_cache[index].lines[i].tag==tag){
            printf("Result HIT !\n");
            my_cache[index].lines[i].last_access_time=current_time;
            return 1;
        }
    }

    for(int i =0;i<4;i++){
    if(my_cache[index].lines[i].valid_bit==0){
    printf("Result MISS ! (Updating new address in %d)\n",i);
    my_cache[index].lines[i].valid_bit=1;
    my_cache[index].lines[i].tag=tag;
    my_cache[index].lines[i].last_access_time=current_time;
    return 0;
    }
    }
   int mintime=my_cache[index].lines[0].last_access_time;
   int victim_way=0;

   for(int i = 1;i<4;i++){
    if(my_cache[index].lines[i].last_access_time<mintime){
        mintime=my_cache[index].lines[i].last_access_time;
        victim_way=i;
    }

   }
   printf("Result: MISS (Cache Full - Evicting Way %d)\n", victim_way);

   my_cache[index].lines[victim_way].valid_bit = 1;
   my_cache[index].lines[victim_way].tag = tag;
   my_cache[index].lines[victim_way].last_access_time = current_time;

   return 0;
 }

int main(){
    unsigned int input_address;
    int total_access=0;
    int hit=0;
    int global_time=0;

    for(int i =0;i<4096;i++){
        for(int j=0;j<4;j++){
            my_cache[i].lines[j].valid_bit=0;
        }
    }

    printf("---- 4-WAY LRU CACHE SIMULATOR ----\n");
    printf("Type address in Hex value 0x12345678 and press enter\n");
    printf("Type 0 to exit\n");
    while (1)
    {
        printf("\n----------------------------------------\n");
        printf("Enter address: 0x");
        scanf("%x",&input_address);

        if(input_address==0){
            printf("--FINAL REPORT--\n");
            printf("Total Accesses: %d\n", total_access);
            printf("Total Hits:     %d\n", hit);
            if(total_access>0){
                float rate=(float)hit/total_access*100;
                printf("Hit Rate :      %.2f%%\n",rate);

            }
            break;
        }
        int result= access_cache(input_address,global_time);

        total_access++;
        if(result==1){
            hit++;
        }

        printf("Current Hit Rate: %.2f%% (%d/%d)\n", 
               (float)hit / total_access * 100.0, hit,total_access);
        global_time++;

    }
    return 0;
}