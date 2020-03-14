/**
 * @file lifegame.c
 * @author Masahiko Hyuga <mail@mhyuga.jp>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/time.h>

double microtime(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

typedef uint64_t pack_t;

#define STRIDE (8*sizeof(pack_t))

#define WIDTH  (8192)
#define HEIGHT (8192)
#define TRY (1)

pack_t **map, **map_next;

static void swap_map(){
	pack_t ** tmp = map;
	map = map_next;
	map_next = tmp;
}

static bool is_alive(int y, int x){
	y = (y + HEIGHT) % HEIGHT;
	x = (x + WIDTH ) % WIDTH;
	return (map[y][x/STRIDE]>>(x%STRIDE)) & 1;
}
#define is_dead(y,x) (!is_alive(y,x))
static void set_life(int y, int x, bool alive){
	y = (y + HEIGHT) % HEIGHT;
	x = (x + WIDTH ) % WIDTH;
	if(alive){
		map_next[y][x/STRIDE] |= 1 << (x%STRIDE);
	}else{
		map_next[y][x/STRIDE] &= ~((pack_t)1 << (x%STRIDE));
	}
}

static void print_map(){
	for(int y=0; y<HEIGHT; y++){
		for(int x=0; x<WIDTH; x++){
			fprintf(stdout, "%c", is_alive(y, x)?'@':'.');
		}
		fprintf(stdout, "\n");
	}
}

static void next_straight(){
	for(int y=0; y<HEIGHT; y++){
		for(int x=0; x<WIDTH; x++){
			int cnt = 0;
			if(is_alive(y-1, x-1)) cnt++;
			if(is_alive(y-1, x  )) cnt++;
			if(is_alive(y-1, x+1)) cnt++;
			if(is_alive(y  , x-1)) cnt++;
			if(is_alive(y  , x+1)) cnt++;
			if(is_alive(y+1, x-1)) cnt++;
			if(is_alive(y+1, x  )) cnt++;
			if(is_alive(y+1, x+1)) cnt++;
			set_life(y, x, ((cnt==3) || ((cnt==2) && is_alive(y, x))));
		}
	}
	swap_map();
}

int main(int argc, char *argv[]){
	assert(WIDTH % STRIDE == 0);
	
	/*
	if(argc < 2){
		fprintf(stderr, "Usage: %s\n", argv[0]);
		return 0;
	}
	*/
	
	double lap;
	
	// Allocate memory.
	lap = microtime();
	fprintf(stderr, "Allocating memory... ");
	map = malloc(sizeof(pack_t*) * HEIGHT);
	map_next = malloc(sizeof(pack_t*) * HEIGHT);
	for(int y=0; y<HEIGHT; y++){
		map[y] = malloc(sizeof(pack_t) * WIDTH / STRIDE);
		map_next[y] = malloc(sizeof(pack_t) * WIDTH / STRIDE);
	}
	fprintf(stderr, "done: %.2fsec\n", microtime()-lap);
	
	// Initialize map.
	lap = microtime();
	fprintf(stderr, "Initializing map... ");
	srand(0);
	for(int y=0; y<HEIGHT; y++){
		for(int x=0; x<WIDTH; x++){
			set_life(y, x, (rand()%3)==0);
		}
	}
	swap_map();
	fprintf(stderr, "done: %.2fsec\n", microtime()-lap);
	
	lap = microtime();
	fprintf(stderr, "Calculating... ");
	for(int i=0; i<TRY; i++){
		next_straight();
	}
	fprintf(stderr, "done: %.2fsec\n", microtime()-lap);
	
	// Free memory.
	for(int y=0; y<HEIGHT; y++){
		free(map[y]);
		free(map_next[y]);
	}
	free(map);
	free(map_next);
	
	return 0;
	
}

