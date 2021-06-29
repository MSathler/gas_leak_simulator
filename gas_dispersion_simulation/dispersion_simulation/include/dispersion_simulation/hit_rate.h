#ifndef HIT_RATE_H
#define HIT_RATE_H

#include <stdio.h>
#include "dispersion_simulation/flow.h"
#include "dispersion_simulation/utility.h"


#define HIT_RATE_COUNT_X 16+1 //sepid 32+1
#define HIT_RATE_COUNT_Y 4+1 //sepid 8+1
#define HIT_RATE_COUNT_Z 1+1 //3d 2+1







/*
#define HIT_RATE_COUNT_X 122 // 60+1sepid 32+1
#define HIT_RATE_COUNT_Y  44 //20+1sepid 8+1
#define HIT_RATE_COUNT_Z 12 //5+13d 2+1
*/
#define HIT_RATE_CELLS_SIZE 1 //sepid 0.5

struct sHitRate {
	struct {
		int x;
		int y;
		int z; //3d
	} cells_count;
	struct {
		double x;
		double y;
		double z;//3d
	} cells_size;
	//double value[HIT_RATE_COUNT_X][HIT_RATE_COUNT_Y][HIT_RATE_COUNT_Z]; //3d
	double ***value;
};

// BEGIN - GENERATED BY C-UPDATE-HEADER
// Initialize the flow structure
void hit_rate_init(struct sHitRate *hit_rate,int,int,int,double);
int hit_rate_add(struct sHitRate *hit_rate, double x, double y, double z, double weight); //3d
void hit_rate_rescale(struct sHitRate *hit_rate, double factor);
void hit_rate_save(struct sHitRate *hit_rate, char *filename);
// END - GENERATED BY C-UPDATE-HEADER

#endif