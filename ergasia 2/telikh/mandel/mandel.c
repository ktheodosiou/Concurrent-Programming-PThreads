#include "mandelcalc.h"

#define WinW 300
#define WinH 300
#define ZoomStepFactor 0.5
#define ZoomIterationFactor 2

static Display *dsp = NULL;
static unsigned long curC;
static Window win;
static GC gc;

/* basic win management rountines */
static void openDisplay() {
	if (dsp == NULL) { 
		dsp = XOpenDisplay(NULL); 
	} 
}

static void closeDisplay() {
	if (dsp != NULL) { 
		XCloseDisplay(dsp); 
		dsp=NULL;
	}
}

void openWin(const char *title, int width, int height) {
	unsigned long blackC,whiteC;
	XSizeHints sh;
	XEvent evt;
	long evtmsk;
	
	whiteC = WhitePixel(dsp, DefaultScreen(dsp));
	blackC = BlackPixel(dsp, DefaultScreen(dsp));
	curC = blackC;
	
	win = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, WinW, WinH, 0, blackC, whiteC);
	
	sh.flags=PSize|PMinSize|PMaxSize;
	sh.width=sh.min_width=sh.max_width=WinW;
	sh.height=sh.min_height=sh.max_height=WinH;
	XSetStandardProperties(dsp, win, title, title, None, NULL, 0, &sh);
	
	XSelectInput(dsp, win, StructureNotifyMask|KeyPressMask);
	XMapWindow(dsp, win);
	do {
		XWindowEvent(dsp, win, StructureNotifyMask, &evt);
	} while (evt.type != MapNotify);
	
	gc = XCreateGC(dsp, win, 0, NULL);
	
}

void closeWin() {
	XFreeGC(dsp, gc);
	XUnmapWindow(dsp, win);
	XDestroyWindow(dsp, win);
}

void flushDrawOps() {
	XFlush(dsp);
}

void clearWin() {
	XSetForeground(dsp, gc, WhitePixel(dsp, DefaultScreen(dsp)));
	XFillRectangle(dsp, win, gc, 0, 0, WinW, WinH);
	flushDrawOps();
	XSetForeground(dsp, gc, curC);
}

void drawPoint(int x, int y) {
	XDrawPoint(dsp, win, gc, x, WinH-y);
	flushDrawOps();
}

void getMouseCoords(int *x, int *y) {
	XEvent evt;
	
	XSelectInput(dsp, win, ButtonPressMask);
	do {
		XNextEvent(dsp, &evt);
	} while (evt.type != ButtonPress);
	*x=evt.xbutton.x; *y=evt.xbutton.y;
}

/* color stuff */

void setColor(char *name) {
	XColor clr1,clr2;
	
	if (!XAllocNamedColor(dsp, DefaultColormap(dsp, DefaultScreen(dsp)), name, &clr1, &clr2)) {
		printf("failed\n"); return;
	}
	XSetForeground(dsp, gc, clr1.pixel);
	curC = clr1.pixel;
}

char *pickColor(int v, int maxIterations) {
	static char cname[128];
	
	if (v == maxIterations) {
		return("black");
	}
	else {
		sprintf(cname,"rgb:%x/%x/%x",v%64,v%128,v%256);
		return(cname);
	}
}

/* to func pairnei timh 1 gia thn init, 2 lock, 3 unlock, 4 destroy. to check pairnei thn timh poy epestrepse h synarthsh*/
int error_check(int check, int func){
	if (check!=0){
		if(func==1){
			printf("Problem with pthread_mutex_init\n");
		}
		else if (func==2){
			printf("Problem with pthread_mutex_lock\n");
		}
		else if (func==3){
			printf("Problem with pthread_mutex_unlock\n");
		}
		else{
			printf("Problem with pthread_mutex_destroy\n");
		}
		exit(8);
	}
	else{
		return(0);
	}
}

int main(int argc, char *argv[]) {
	
	int i,j,k,x,y,level;
	int xoff,yoff;
	int check;
	int sliceImPixels;
	long double Step;
	long double reStep,imStep;
	pthread_t *threads;
	
	printf("\n");
	printf("This program starts by drawing the default Mandelbrot region\n");
	printf("When done, you can click with the mouse on an area of interest\n");
	printf("and the program will zoom at this point\n");
	printf("\n");
	printf("Press enter to continue\n");
	getchar();
	
	pars = (mandelPars *)malloc(sizeof(mandelPars));
	if(pars==NULL){												//elegxos epityxias ths malloc
		printf("Problem with memory allocation\n");
		return(1);
	}
	
	pars->rePixels = WinW; /* never changes */
	pars->imPixels = WinH; /* never changes */
	
	printf("enter max iterations (50): ");
	scanf("%d",&pars->maxIterations);
	printf("enter no of slices: ");
	scanf("%d",&pars->slices);
	
	/* adjust slices to divide win height */
	
	while (WinH % pars->slices != 0) { pars->slices++;}
	pars->imBeg = (long double*)malloc(sizeof(long double)*(pars->slices));
	if(pars->imBeg==NULL){												//elegxos epityxias ths malloc
		printf("Problem with memory allocation\n");
		free(pars);
		return(2);
	}
	pars->rdy = (int *)malloc(sizeof(int)*(pars->slices));
	if(pars->rdy==NULL){												//elegxos epityxias ths malloc
		printf("Problem with memory allocation\n");
		free(pars->imBeg);
		free(pars);
		return(3);
	}
	/* default mandelbrot region */
	
	pars->reBeg = (long double) -2.0;
	pars->reEnd = (long double) 1.0;
	pars->imBeg[0] = (long double) -1.5;
	pars->imEnd = (long double) 1.5;
		
	reStep = (pars->reEnd - pars->reBeg) / pars->rePixels;
	imStep = (pars->imEnd - pars->imBeg[0]) / pars->imPixels;
	
	/* allocate result and ready arrays */
	res = (int *) malloc(sizeof(int)*pars->rePixels*pars->imPixels);
	if(res==NULL){													//elegxos epityxias ths malloc
		printf("Problem with memory allocation\n");
		free(pars->imBeg);
		free(pars->rdy);
		free(pars);
		return(4);
	}
	
	array_k = (int *)malloc(sizeof(int)*(pars->slices));
	if(array_k==NULL){												//elegxos epityxias ths malloc
		printf("Problem with memory allocation\n");
		free(pars->imBeg);
		free(pars->rdy);
		free(pars);
		free(res);
		return(5);
	}
	
	/* set wait state */
	check = pthread_mutex_init(&mtx,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&safe_mtx,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&blck,NULL);
	error_check(check,1);
	check = pthread_mutex_init(&safe_blck,NULL);
	error_check(check,1);
	
	check = pthread_mutex_lock(&mtx);
	error_check(check,2);
	check = pthread_mutex_lock(&safe_mtx);
	error_check(check,2);
	check = pthread_mutex_lock(&blck);
	error_check(check,2);
	
	threads = (pthread_t *)malloc(sizeof(pthread_t)*(pars->slices));
	if(threads==NULL){												//elegxos epityxias ths malloc
		printf("Problem with memory allocation\n");
		free(pars->imBeg);
		free(pars->rdy);
		free(pars);
		free(res);
		free(array_k);
		return(6);
	}
	
	i=0;
	while(i<pars->slices) {
		pars->rdy[i] = 0;
		array_k[i] = i;			//timh opou deixnei ton arithmo tou kathe thread
		check = pthread_create(&threads[i], NULL, &calcMandel, (void *) &array_k[i]);
		if(check!=0) {
			printf("Problem to create %d thread\n", i);
			return(7);
		}
		i++;
	}
	
	/* open window for drawing results */
	openDisplay();
	openWin(argv[0], WinW, WinH);
	
	level = 1;
	
	while (1) {
		
		clearWin();
		
		printf("computing for level %d:\n",level);
		printf("reBeg=%Lf reEnd=%Lf reStep=%Lf\n",pars->reBeg,pars->reEnd,reStep);
		printf("imBeg=%Lf imEnd=%Lf imStep=%Lf\n",pars->imBeg[0],pars->imEnd,imStep);
		printf("maxIterations=%d\n",pars->maxIterations);
		
		//ginetai h anathesh tou imBeg edo dioti to kathe thread prepei na exei diaforetiko imBeg
		//to Step kai to sliceImPixels exoyme parei ton kodika apo thn mandelcalc oste na mporei na ypologistei to imBeq
		Step = (pars->imEnd - pars->imBeg[0]) / pars->imPixels;
		sliceImPixels = pars->imPixels / pars->slices;
		
		for (i=1; i<pars->slices; i++) { 
			pars->imBeg[i] =  pars->imBeg[i-1] + Step*sliceImPixels;
		}
		for (i=0; i<pars->slices; i++) { 
			check = pthread_mutex_unlock(&mtx);
			error_check(check,3);
			check = pthread_mutex_lock(&safe_mtx);
			error_check(check,2);
		} 
		
		/* busywait (and draw results) until all slices done */
		for (j=0; j<pars->slices; j++) {
			check = pthread_mutex_lock(&blck);
			error_check(check,2);
			
			for (i=0; i<pars->slices; i++) {
				if(pars->rdy[i]==1){
					for (y=i*(pars->imPixels/pars->slices); y<(i+1)*(pars->imPixels/pars->slices); y++) {
						for (x=0; x<pars->rePixels; x++) {
							setColor(pickColor(res[y*pars->rePixels+x],pars->maxIterations));
							drawPoint(x,y);
						}
					}
					pars->rdy[i] = 0;
					break;
				}
			}
			check = pthread_mutex_unlock(&safe_blck);
			
			
		}
		
		
		/* get next focus/zoom point */
		
		getMouseCoords(&x,&y);
		xoff = WinW/2 - x;
		yoff = WinH/2 - (WinH-y);
		
		/* adjust focus */
		
		pars->reBeg = pars->reBeg - xoff*reStep;
		pars->reEnd = pars->reBeg + reStep*pars->rePixels;
		pars->imBeg[0] = pars->imBeg[0] - yoff*reStep;
		pars->imEnd = pars->imBeg[0] + reStep*pars->imPixels;
		
		/* zoom in */
		
		reStep = reStep*ZoomStepFactor; 
		imStep = imStep*ZoomStepFactor;
		pars->reBeg = pars->reBeg + (pars->reEnd-pars->reBeg)/2 - reStep*pars->rePixels/2;
		pars->reEnd = pars->reBeg+reStep*pars->rePixels;
		pars->imBeg[0] = pars->imBeg[0] + (pars->imEnd-pars->imBeg[0])/2 - imStep*(pars->imPixels)/2;
		pars->imEnd = pars->imBeg[0]+imStep*pars->imPixels;
		pars->maxIterations = pars->maxIterations*ZoomIterationFactor;
	
		level++;
	
	} 
	/* never reach this point; for cosmetic reasons */
	free(pars->imBeg);
	free(pars->rdy);
	free(pars);
	free(res);
	free(array_k);
	free(threads);
	
	/*destroy ta mutexes*/
	check = pthread_mutex_destroy(&mtx);
	error_check(check,4);
	check = pthread_mutex_destroy(&safe_mtx);
	error_check(check,4);
	check = pthread_mutex_destroy(&blck);
	error_check(check,4);
	check = pthread_mutex_destroy(&safe_blck);
	error_check(check,4);
	
	closeWin();
	closeDisplay();
	
}
