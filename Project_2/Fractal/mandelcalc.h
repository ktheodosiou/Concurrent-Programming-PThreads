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
	int *rdy;
} mandelPars;

/*tis dhlonoyme global oste na mporoyn na tis doyn ta threads*/
mandelPars *pars;
int *res;
int *array_k;

/*dhlwnomou ola ta mutexes*/
pthread_mutex_t mtx;   //to mtx einai upeuthuno gia thn afupnish twn threads
pthread_mutex_t safe_mtx;  //to safe_mtx mas ebodizei na xasoume unlocks kai mas eksasfalizei th swsth kataxwrhsh dedomenwn sta threads
pthread_mutex_t blck;   //mas deixnei oti kapoio thread exei teleiwsei
pthread_mutex_t safe_blck;  //mas eksasfalizei oti den tha xathei kapoio shma block
/* assume imPixels % slices == 0 */

extern void * calcMandel(void *number);//to number einai o arithmos pou adistoixei se kathe thread
