#include "mandelcalc.h"

/* sub-region parameters */
typedef struct {
	long double reBeg,reStep;  
	long double imBeg,imStep;
	int rePixels,imPixels;
	int maxIterations;
	int *res; /* array where to store result values */
	int *rdy;
} sliceMPars;

/* the mandelbrot test */

static int isMandelbrot(double re, double im, int maxIterations) {
	long double zr = 0.0, zi = 0.0;
	long double xsq = 0.0, ysq = 0.0;
	long double cr = re, ci = im;
	int n = 0;
	
	while(xsq + ysq < 4.0 && n < maxIterations) {
		xsq = zr * zr;
		ysq = zi * zi;
		zi = 2 * zr * zi + ci;
		zr = xsq - ysq + cr;
		n++;
	}
	return(n);
}

/* perform mandelbrot computation for a sub-region */

static void computeMandelbrot(sliceMPars *p) {
	long double re,im; 
	int x,y;
	
	im = p->imBeg;
	for (y=0; y<p->imPixels; y++) {
		re = p->reBeg;
		for (x=0; x<p->rePixels; x++) {
		p->res[y*p->rePixels+x] = isMandelbrot (re,im,p->maxIterations);
		re = re + p->reStep;
		}
		im = im + p->imStep;
	}
	*(p->rdy) = 1;
}

/* perform computation for an entire region */
void * calcMandel (void *number) {
	int k;
	int check;
	long double imBeg;
	long double reStep;
	long double imStep;
	int sliceImPixels;
	int sliceTotPixels;
	
	/* allocate array for sub-region parameters */
	sliceMPars *p = (sliceMPars *)malloc(sizeof(sliceMPars));
	
	
	/* fill in paramters for each sub-region */
	while(1){
		check = pthread_mutex_lock(&mtx);	//kleidonoume mexri na mas pei h main na paroume dedomena
		error_check(check,2);
		
		k = *((int*)number);
		reStep = (pars->reEnd - pars->reBeg) / pars->rePixels;
		imStep = (pars->imEnd - pars->imBeg[0]) / pars->imPixels;
		sliceImPixels = pars->imPixels / pars->slices;
		sliceTotPixels = sliceImPixels * pars->rePixels;
		
		imBeg = pars->imBeg[k];
		
		p->reBeg = pars->reBeg;
		p->reStep = reStep;
		p->rePixels = pars->rePixels;
		p->imBeg = imBeg;
		p->imStep = imStep;
		p->imPixels = sliceImPixels;
		p->maxIterations = pars->maxIterations;
		p->res = &res[k*sliceTotPixels];
		p->rdy = &pars->rdy[k];
		
		/*energopoioyme th main legontas oti diabasame ta dedomena maw mporei na energopoihsei to epomeno thread*/
		check = pthread_mutex_unlock(&safe_mtx);	
		error_check(check,3);
		
		/* start computation: see notes below */
		computeMandelbrot(p);
		
		/*kleidonoume oste na perimenoume th na pei oti thelei ta epomena dedomena*/
		check = pthread_mutex_lock(&safe_blck);	
		error_check(check,2);
		/*stelnoume shma sth main legontas oti mporei na parei ta dedomena mas kai na ta epeksergastei*/
		check = pthread_mutex_unlock(&blck);	
		error_check(check,3);
	}
	
	/* release parameter array */
	free(p);
	return(NULL);
}

