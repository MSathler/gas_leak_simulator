
#ifdef __cplusplus
 extern "C" {
 #endif

#ifndef SIMULATION_H
#define SIMULATION_H

#include "dispersion_simulation/filament.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/select.h>

#define	ETHANOL_ID		0
#define	METHANE_ID		1
#define	HYDROGEN_ID		2
#define	PROPANOL_ID		3
#define	CHLORIDE_ID		4
#define	FLURORINE_ID	5
#define	ACETONE_ID		6
#define	NEON_ID			7
#define	HELIUM_ID		9
#define	HOTAIR_ID		10


#define	HIGH_CONCENTRATION_THR	2000
#define	MID_CONCENTRATION_THR	500
#define	LOW_CONCENTRATION_THR	100

  extern float density;
//  extern int option;
  extern float Chemicalvalue[10];

  extern double red;
  extern double green;
  extern double blue;

  //double concentration(int xx, int yy, int zz);
  void plume_start();
  void color_width(double *aa, double *bb, double *cc);
  void color_concentration(double *aa, double *bb, double *cc);

#endif

#ifdef __cplusplus
 }
 #endif
