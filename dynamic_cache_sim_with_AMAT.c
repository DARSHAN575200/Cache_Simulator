#include<stdio.h>
#include<stdlib.h>
#include<math.h>
struct cache_line
{
    int valid_bit;
    unsigned int tag;
    int last_access_time;
    int dirty_bit;

};
struct cache_set
{
   struct cache_line *lines;
};

typedef struct 
{
    struct cache_set *sets;
    int num_sets;
    int associativity;
    int block_size;
    int index_bits;
    int offset_bits;

}CacheSystem;

CacheSystem *cache=NULL;

int calculate_bits(int size){
    int bits=0;
    while((1<<bits)<size){
        bits++;
    }
    return bits;
}

int initialize_cache(int num_sets,int associativity){
    cache= (CacheSystem *)malloc(sizeof(CacheSystem));
    cache ->block_size=64;
    cache ->offset_bits=6;
    cache ->associativity=associativity;
    cache ->num_sets=num_sets;


    cache ->sets = (struct cache_set*)malloc(cache->num_sets *sizeof(struct cache_set));

    cache->index_bits=calculate_bits(num_sets);

    for(int i =0;i<cache->num_sets;i++){
        cache->sets[i].lines=(struct cache_line*)malloc(associativity*sizeof(struct cache_line));
        for(int j =0;j<associativity;j++){
            cache->sets[i].lines[j].valid_bit=0;
            cache->sets[i].lines[j].dirty_bit=0;
            cache->sets[i].lines[j].tag=0;
            cache->sets[i].lines[j].last_access_time=0;
        }
        
    }


}
int access_cache(int address,char type,int current_time){
  
    int index_bits = cache->index_bits;
    int offset_bits = cache->offset_bits;

    unsigned int index_mask = (1 <<index_bits) - 1;

    unsigned int offset = address & ((1 << offset_bits) - 1);
    unsigned int index = (address >> offset_bits) & index_mask;
    
    unsigned int tag = address >> (offset_bits + index_bits);
    

    printf("\nchecking address: 0x%08x  |Tag : %d  | index : 0x%x\n",address,tag,index);

    for(int i =0;i<cache->associativity;i++){
        if(cache->sets[index].lines[i].valid_bit==1 && cache->sets[index].lines[i].tag==tag){
            printf("Result HIT !\n");
            cache->sets[index].lines[i].last_access_time=current_time;

            if(type=='W'){
                cache->sets[index].lines[i].dirty_bit=1;
                printf("-> Mark Dirty: YES\n");
            }
            return 1;
        }
    }

    for(int i =0;i<cache->associativity;i++){
    if(cache->sets[index].lines[i].valid_bit==0){
    printf("Result MISS ! (Updating new address in %d)\n",i);
    cache->sets[index].lines[i].valid_bit=1;
    cache->sets[index].lines[i].tag=tag;
    cache->sets[index].lines[i].last_access_time=current_time;
    if(type=='W'){
        cache->sets[index].lines[i].dirty_bit=1;
        printf("-> Mark Dirty : Yes\n");
    }else{
        cache->sets[index].lines[i].dirty_bit=0;
        printf("-> Mark Dirty : No\n");
    }
    return 0;
    }
    }
   int mintime=cache->sets[index].lines[0].last_access_time;
   int victim_way=0;

   for(int i = 1;i<cache->associativity;i++){
    if(cache->sets[index].lines[i].last_access_time<mintime){
        mintime=cache->sets[index].lines[i].last_access_time;
        victim_way=i;
    }

   }

   printf("Result: MISS (Cache Full - Evicting Way %d)\n", victim_way);

   if(cache->sets[index].lines[victim_way].dirty_bit==1){
    printf("  -> WRITING BACK dirty block to RAM... (Slow!)\n");
   }

   cache->sets[index].lines[victim_way].valid_bit = 1;
   cache->sets[index].lines[victim_way].tag = tag;
   cache->sets[index].lines[victim_way].last_access_time = current_time;

   if(type=='W'){
    cache->sets[index].lines[victim_way].dirty_bit=1;
   }else{
    cache->sets[index].lines[victim_way].dirty_bit=0;
   }

   return 0;
 }

int main(int argc,char *argv[]){
    if(argc !=3){
        printf("Error: Usage is ./cache_sim <num_sets> <associativity>\n");
        return 1;
    }

    int sets_input=atoi(argv[1]);
    int ways_input=atoi(argv[2]);
    printf("Starting Simulator with %d Sets and %d Ways...\n", sets_input, ways_input);

    initialize_cache(sets_input,ways_input);

    unsigned int input_address;
    char access_type;
    int total_access=0;
    int hit=0;
    int global_time=0;
    const int Hit_Latency=1;
    const int Miss_Latency=100;
    long  long total_cycles=0;  //long long because CPUs run for billions of cycles, and a normal integer might overflow

    printf("---- CACHE SIMULATOR ----\n");
    FILE *fp=fopen("trace.txt","r");
    if(fp==NULL){
        return 1;
    }
    printf("Reading trace.txt...\n");
    while (fscanf(fp," %c %x",&access_type,&input_address)==2)
    {
        
        int result= access_cache(input_address,access_type,global_time);

        total_access++;
        if(result==1){
            hit++;
            total_cycles+=Hit_Latency;
        }else{
            total_cycles+=Miss_Latency;
        }

        printf("Current Hit Rate: %.2f%% (%d/%d)\n", 
               (float)hit / total_access * 100.0, hit,total_access);
        global_time++;

    }
    fclose(fp);
    printf("\n----------------------------------------\n");
    printf("-- FINAL STATS --\n");
    printf("Total Accesses: %d\n", total_access);
    printf("Total Hits:     %d\n", hit);
    printf("Total Misses:   %d\n", total_access - hit);
    if (total_access > 0) {
        printf("Hit Rate:       %.2f%%\n", (float)hit / total_access * 100.0);
        double amat=(double)total_cycles/total_access;
        printf("---------------------------\n");
        printf("Total Cycles:   %lld\n", total_cycles);
        printf("AMAT:           %.2f cycles/access\n", amat);
    }
    return 0;
}