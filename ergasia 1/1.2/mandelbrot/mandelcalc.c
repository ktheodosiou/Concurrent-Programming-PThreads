#include "mandelcalc.h"
#include <stdlib.h>

/* sub-region parameters */

typedef struct {
  long double reBeg,reStep;  
  long double imBeg,imStep;
  int rePixels,imPixels;
  int maxIterations;
  int *res; /* array where to store result values */
  int *rdy; /* set 1 when done */
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

  *(p->rdy)=1;

}

/* perform computation for an entire region */

void calcMandel (mandelPars *pars, int *res, int *rdy) {
  int k;
  long double imBeg;
  long double reStep = (pars->reEnd - pars->reBeg) / pars->rePixels;
  long double imStep = (pars->imEnd - pars->imBeg) / pars->imPixels;
  int sliceImPixels = pars->imPixels / pars->slices;
  int sliceTotPixels = sliceImPixels * pars->rePixels;

  /* allocate array for sub-region parameters */

  sliceMPars *p = (sliceMPars *)malloc(pars->slices * sizeof(sliceMPars));

  /* fill in paramters for each sub-region */

  imBeg = pars->imBeg;
  for (k=0; k<pars->slices; k++) {
    p[k].reBeg = pars->reBeg;
    p[k].reStep = reStep;
    p[k].rePixels = pars->rePixels;
    p[k].imBeg = imBeg;
    p[k].imStep = imStep;
    p[k].imPixels = sliceImPixels;
    p[k].maxIterations = pars->maxIterations;
    p[k].res = &res[k*sliceTotPixels];
    p[k].rdy = &rdy[k];
    imBeg =  imBeg + imStep*sliceImPixels;
  }

  /* start computation: see notes below */

  for (k=0; k<pars->slices; k++) {
    computeMandelbrot(&p[k]); 
  }

  /* release parameter array */

  free(p);

}

