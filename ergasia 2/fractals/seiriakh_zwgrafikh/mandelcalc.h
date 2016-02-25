#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include<pthread.h>

typedef struct {
	long double reBeg,reEnd;  /* range of real values */
	long double *imBeg,imEnd;  /* range of imaginary values to imBeg mphke san pointer oste na ginei pinakas kai to kathe thread na exei diko toy */
	int rePixels,imPixels;    /* number of pixels per range */
	int maxIterations;        /* iteration cut off value */
	int slices;               /* number of sub-computations */
} mandelPars;

//tis dhlonoyme global oste na mporoyn na tis doyn ta threads
mandelPars *pars;
int *res;
int *array_k;

pthread_mutex_t *lck_t;
pthread_mutex_t *lck_m;
/* assume imPixels % slices == 0 */

extern void * calcMandel(void *number);//to number einai o arithmos kathe thread
