typedef struct {
	long double reBeg,reEnd;  /* range of real values */
	long double imBeg,imEnd;  /* range of imaginary values */
	int rePixels,imPixels;    /* number of pixels per range */
	int maxIterations;        /* iteration cut off value */
	int slices;               /* number of sub-computations */
} mandelPars;
mandelPars *pars;

int *res;
int *rdy;
int *array_k;
/* assume imPixels % slices == 0 */

extern void * calcMandel(void *i);
