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
int *rdy;
int *array_k;
/* assume imPixels % slices == 0 */

extern void * calcMandel(void *number);//to number einai o arithmos kathe thread
