#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#define __HINT do{ \
		printf("Usage: ./csim-test [-hv] -s <num> -E <num> -b <num> -t <file>/nOptions:\n  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n  -t <file>  Trace file.\n\nExamples:\n  linux>  ./csim-test -s 4 -E 1 -b 4 -t traces/yi.trace\n  linux>  ./csim-test -v -s 8 -E 2 -b 4 -t traces/yi.trace\n"); \
		return false; \
	}while(0)
#define __MISS_ARGUMENT printf("./csim-test: Missing required command line argument\n")

//global variables made by getopt()
extern char *optarg;
extern int optind,opterr,optopt;

typedef struct Slot_S {
  unsigned long long tag;
  struct Slot_S *next;
  struct Slot_S *pre;
} Slot;

typedef struct {
  int num;
  Slot *first;
  Slot *last;
} Slot_list;

typedef struct {
	int hit_count;
	int miss_count;
	int eviction_count; 
} Result;

int convert_str_to_int(char* str);

bool get_option(int argc, char** argv, int* p_s, int* p_E, int* p_b, int* p_v, char* file); 

Result simulate(int s, int E, int b, int v, const char *file);

//void printSummary(int hit_count, int miss_count, int eviction_count);

int main(int argc, char **argv)
{
	int s,E,b;
	int v = 0;
	Result res;
	char file[256];
	s = E = b = -1;
	if(!get_option(argc, argv, &s, &E, &b, &v, file)) return 0;

	res = simulate(s, E, b, v, file);
	
	printSummary(res.hit_count, res.miss_count, res.eviction_count);
	return 0;
}

bool get_option(int argc, char** argv, int* p_s, int* p_E, int* p_b, int* p_v, char* file){
	int op;
	while((op=getopt(argc,argv,"hvs:E:b:t:"))!=-1){
		switch(op){
			case 'v':
				*p_v = 1;
				break;
			case 's':
				if((*p_s = convert_str_to_int(optarg))==-1){
					__MISS_ARGUMENT;
					__HINT;
				}
				break;
			case 'E':
				if((*p_E = convert_str_to_int(optarg))==-1){
					__MISS_ARGUMENT;
					__HINT;
				}
				break;
			case 'b':	
				if((*p_b = convert_str_to_int(optarg))==-1){
					__MISS_ARGUMENT;
					__HINT;
				}
				break;
			case 't':
				if(strlen(optarg)>255) return false;
				strncpy(file,optarg,255);
				break;
			case '?':
			case ':':
			case 'h':
				__HINT;
		}
	}
	if(*p_s==-1 || *p_E==-1 || *p_b==-1 || file==NULL){
		__MISS_ARGUMENT;
		__HINT;
	}

	return true;
}
 
int convert_str_to_int(char* str){
	int len = strlen(str);
	int i;
	int num,res=0;
	for(i=0;i<len;i++){
		num = *str++;
		if(num>=48 && num <58){
			res = res*10 + num - 48;
		}else{
			return -1;
		}
	}
	return res;
}

Result simulate(int s, int E, int b, int v, const char *file){
	char buffer[1024];
	char ins,op;
	unsigned long long addr;
	int bytes;
	int s_id;
	unsigned long long tag;
	int i;
	int is_hit=0,is_miss=0,is_eviction=0;
	Slot_list *cache;
	Slot *t,*tem;
	FILE *test;
	Result res;
	res.eviction_count = res.hit_count = res.miss_count = 0;
	//init
	cache = malloc((1<<s)*sizeof(Slot_list));
	for(i=0;i<(1<<s);i++){
		cache[i].first = cache[i].last = NULL;
		cache[i].num = 0;
	}
	
	//read
	test = fopen(file, "r");
	while(fgets(buffer,1023,test)){
		sscanf(buffer,"%c",&ins);
		if(ins!=' ') continue;
		
		sscanf(buffer+1,"%c %llx,%d",&op,&addr,&bytes);
		
		if(op=='S'||op=='L'||op=='M'){
			s_id = (addr>>b) & ((1<<s)-1);
			tag = (addr>>b) & ((unsigned long long)0-(1<<s));
			
			for(t=cache[s_id].first;t!=NULL;t=t->next){
				if(t->tag==tag){
					is_hit = 1;
					res.hit_count++;
					
					if(t!=cache[s_id].first){
						if(t==cache[s_id].last){
							cache[s_id].last = t->pre;
						}else{
							t->next->pre = t->pre;
						}
						t->pre->next = t->next;
						cache[s_id].first->pre = t;
						t->next = cache[s_id].first;
						t->pre = NULL;
						cache[s_id].first = t;
					}
					break;
				}
			}
			if(!is_hit){//miss
				is_miss = 1;
				res.miss_count++;
				if(cache[s_id].num < E){
					tem = malloc(sizeof(Slot));
					tem->tag = tag;
					tem->next = cache[s_id].first;
					tem->pre = NULL;
					if(cache[s_id].first)  cache[s_id].first->pre = tem;
					if(cache[s_id].last==NULL){
						cache[s_id].first = cache[s_id].last = tem;
					}else{
						cache[s_id].first = tem;
					}
					cache[s_id].num++;
				}else{
					is_eviction = 1;
					res.eviction_count++;
					cache[s_id].last->tag = tag;
					
					if(E>1){
						cache[s_id].last->pre->next = NULL;
						cache[s_id].last->next = cache[s_id].first;
						cache[s_id].first->pre  = cache[s_id].last;
						cache[s_id].first = cache[s_id].last;
						cache[s_id].last = cache[s_id].last->pre;
						cache[s_id].first->pre = NULL;
					}
				}
			}
			if(op=='M'){
			  res.hit_count++;
			}
			//output
			if(v){
				printf("%c %llx   \t %d,%d",op,tag,s_id,bytes);
				if(is_miss) printf(" miss");
				if(is_eviction) printf(" eviction");
				if(is_hit) printf(" hit");
				if(op=='M') printf(" hit");
				printf("\n");
			}
			
			is_hit = is_miss = is_eviction = 0;
		}
	}
	return res;
} 

//void printSummary(int hit_count, int miss_count, int eviction_count){
//	printf("hits:%d misses:%d evictions:%d\n", hit_count, miss_count, eviction_count);
//}
