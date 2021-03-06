#include "mex.h"
#include <math.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
     /* Declarations */
     const mxArray *coefsData, *parData, *spInfoData, *knotsData, *indexData;
    
    double *coefs, *params, *spInfo, *knots, *index;
    int N, splpieces, id, vecLength, sn;
    double Delta, frR, nf;
    double splstart, splend, offsetjump, splos, tr, curt, curt_2, t, obj;
    double ax, bx, cx, dx, ay, by, cy, dy;
    int i, ind, cnt, os;
    int knoffset, indexoffset;
    double tmp1, tmp2, tmp3;
   double px, py, pxs, pys, tsq,  sqrtpxspys, frRsq, twofrRsq, pxspyssq, pxspyscu;
	double px_, py_, px__, py__, px_spy_ssq, k,ksq,kqu,_1_ksq, epsil;
 
    int indtmp;
    double brtmp;
    int allcnt;
    
    double *fx, *dfx;
    
    double t1,t2,t3,t4,t5,t6,c1x,c2x,c3x,c4x,c1y,c2y,c3y,c4y;
    double cmt1,cmt2,cmt3,cmt4,cmt5,cmt6;
    double t14,t24,t25,t35,t36;
    double c2x_cmt1_c1x_cmt4, c2y_cmt1_c1y_cmt4;
    int *svpos;
     
     /* Copy input pointers */
     coefsData = prhs[0];    /* state vec */
    parData = prhs[1];      /* speed, frame rate */
    spInfoData = prhs[2];   /* pieces, starts, ends */
    knotsData = prhs[3];   /* knots */
    indexData = prhs[4];    /* index */
    
    coefs  = mxGetPr(coefsData);
    params = mxGetPr(parData);
    spInfo = mxGetPr(spInfoData);
    knots = mxGetPr(knotsData);
    index  = mxGetPr(indexData);
    
    /* How many coefficients? */
    vecLength = (int)mxGetM(coefsData);
    
     /* How many splines? */
     N = (int)mxGetM(spInfoData);     
     mxAssert((int)mxGetN(spInfoData)==4, "spInfo must have 4 columns");
     

     /* Get parameters */
     Delta = params[0];
     
     
     /* // Allocate memory and assign output pointer */
      plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
      plhs[1] = mxCreateDoubleMatrix(vecLength, 1, mxREAL);
     
     /* //Get a pointer to the data space in our newly allocated memory  */
      fx = mxGetPr(plhs[0]);
      dfx = mxGetPr(plhs[1]);
     
     offsetjump=8; splos=0;
     knoffset = 0;
     indexoffset = 0;
     
      *fx=0;
     for (id = 0; id < vecLength; id++) {
         dfx[id]=0;
     }
     
     svpos = (int*)malloc(8 * sizeof(int));
     
     
    /* for each target */
      allcnt=0;
     for (id = 0; id < N; id++) {
        
        /* get spline Info */
        os = (int)spInfo[id];
        splstart =  spInfo[id+N];
        splend =    spInfo[id+2*N];
        splpieces = spInfo[id+3*N];
	
	sn=splpieces+3;
	
// 	mexPrintf("%d\n",sn);
	
	
        /* for all frames */
        cnt=0;
        for (tr = splstart; tr <= splend; tr += 1.)
        {
	    curt=tr;
	    indtmp=(int)index[cnt+indexoffset]-1;
	    
// 	    mexPrintf("%.1f %d %d %d\n",tr,indtmp-2+knoffset,indtmp+3+knoffset,knoffset);
	    
	    if (tr==splstart || tr==splend){
	      if (tr==splstart)
		curt_2=tr-Delta;
	      else
		curt_2=tr+Delta;
	      
	    
	    svpos[0]=os+indtmp+0;
	    svpos[1]=os+indtmp+1;
	    svpos[2]=os+indtmp+2;
	    svpos[3]=os+indtmp+3;
	    svpos[4]=os+indtmp+0+sn;
	    svpos[5]=os+indtmp+1+sn;
	    svpos[6]=os+indtmp+2+sn;
	    svpos[7]=os+indtmp+3+sn;
	    
	    indtmp += 3;
	    t1=knots[indtmp-2+knoffset];
	    t2=knots[indtmp-1+knoffset];
	    t3=knots[indtmp+0+knoffset];
	    t4=knots[indtmp+1+knoffset];
	    t5=knots[indtmp+2+knoffset];
	    t6=knots[indtmp+3+knoffset];
	    
	    c1x=coefs[svpos[0]];c2x=coefs[svpos[1]];c3x=coefs[svpos[2]];c4x=coefs[svpos[3]];
	    c1y=coefs[svpos[4]];c2y=coefs[svpos[5]];c3y=coefs[svpos[6]];c4y=coefs[svpos[7]];
	    
	    
	    
	    cmt1=curt-t1;cmt2=curt-t2;cmt3=curt-t3;cmt4=curt-t4;cmt5=curt-t5;cmt6=curt-t6;
	    t14=t1-t4;        t24=t2-t4;        t25=t2-t5;        t35=t3-t5;        t36=t3-t6;
	    c2x_cmt1_c1x_cmt4=(c2x*cmt1 - c1x*cmt4);
	    c2y_cmt1_c1y_cmt4=(c2y*cmt1 - c1y*cmt4);
	    obj= pow(((((c2x*(curt-t1)-c1x*(curt-t4))*(curt-t4))/(t1-t4)-((c3x*(curt-t2)-c2x*(curt-t5))*(curt-t2))/(t2-t5))/(t2-t4)-(((c3x*(curt-t2)-c2x*(curt-t5))*(curt-t5))/(t2-t5)-((c4x*(curt-t3)-c3x*(curt-t6))*(curt-t3))/(t3-t6))/(t3-t5)+((curt-t4)*((c2x*(curt-t1)-c1x*(curt-t4))/(t1-t4)-(c3x*(curt-t2)-c2x*(curt-t5))/(t2-t5)-((c1x-c2x)*(curt-t4))/(t1-t4)+((c2x-c3x)*(curt-t2))/(t2-t5)))/(t2-t4)-((curt-t3)*((c3x*(curt-t2)-c2x*(curt-t5))/(t2-t5)-(c4x*(curt-t3)-c3x*(curt-t6))/(t3-t6)-((c2x-c3x)*(curt-t5))/(t2-t5)+((c3x-c4x)*(curt-t3))/(t3-t6)))/(t3-t5))/(t3-t4)-((((c2x*(curt_2-t1)-c1x*(curt_2-t4))*(curt_2-t4))/(t1-t4)-((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t2))/(t2-t5))/(t2-t4)-(((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t5))/(t2-t5)-((c4x*(curt_2-t3)-c3x*(curt_2-t6))*(curt_2-t3))/(t3-t6))/(t3-t5)+((curt_2-t4)*((c2x*(curt_2-t1)-c1x*(curt_2-t4))/(t1-t4)-(c3x*(curt_2-t2)-c2x*(curt_2-t5))/(t2-t5)-((c1x-c2x)*(curt_2-t4))/(t1-t4)+((c2x-c3x)*(curt_2-t2))/(t2-t5)))/(t2-t4)-((curt_2-t3)*((c3x*(curt_2-t2)-c2x*(curt_2-t5))/(t2-t5)-(c4x*(curt_2-t3)-c3x*(curt_2-t6))/(t3-t6)-((c2x-c3x)*(curt_2-t5))/(t2-t5)+((c3x-c4x)*(curt_2-t3))/(t3-t6)))/(t3-t5))/(t3-t4),2.0)+pow(((((c2y*(curt-t1)-c1y*(curt-t4))*(curt-t4))/(t1-t4)-((c3y*(curt-t2)-c2y*(curt-t5))*(curt-t2))/(t2-t5))/(t2-t4)-(((c3y*(curt-t2)-c2y*(curt-t5))*(curt-t5))/(t2-t5)-((c4y*(curt-t3)-c3y*(curt-t6))*(curt-t3))/(t3-t6))/(t3-t5)+((curt-t4)*((c2y*(curt-t1)-c1y*(curt-t4))/(t1-t4)-(c3y*(curt-t2)-c2y*(curt-t5))/(t2-t5)-((c1y-c2y)*(curt-t4))/(t1-t4)+((c2y-c3y)*(curt-t2))/(t2-t5)))/(t2-t4)-((curt-t3)*((c3y*(curt-t2)-c2y*(curt-t5))/(t2-t5)-(c4y*(curt-t3)-c3y*(curt-t6))/(t3-t6)-((c2y-c3y)*(curt-t5))/(t2-t5)+((c3y-c4y)*(curt-t3))/(t3-t6)))/(t3-t5))/(t3-t4)-((((c2y*(curt_2-t1)-c1y*(curt_2-t4))*(curt_2-t4))/(t1-t4)-((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t2))/(t2-t5))/(t2-t4)-(((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t5))/(t2-t5)-((c4y*(curt_2-t3)-c3y*(curt_2-t6))*(curt_2-t3))/(t3-t6))/(t3-t5)+((curt_2-t4)*((c2y*(curt_2-t1)-c1y*(curt_2-t4))/(t1-t4)-(c3y*(curt_2-t2)-c2y*(curt_2-t5))/(t2-t5)-((c1y-c2y)*(curt_2-t4))/(t1-t4)+((c2y-c3y)*(curt_2-t2))/(t2-t5)))/(t2-t4)-((curt_2-t3)*((c3y*(curt_2-t2)-c2y*(curt_2-t5))/(t2-t5)-(c4y*(curt_2-t3)-c3y*(curt_2-t6))/(t3-t6)-((c2y-c3y)*(curt_2-t5))/(t2-t5)+((c3y-c4y)*(curt_2-t3))/(t3-t6)))/(t3-t5))/(t3-t4),2.0);
*fx+=obj;
dfx[svpos[0]] += ((pow(curt-t4,2.0)*3.0)/(t14*t24*(t3-t4))-(pow(curt_2-t4,2.0)*3.0)/(t14*t24*(t3-t4)))*(((((c2x*cmt1-c1x*cmt4)*cmt4)/t14-((c3x*cmt2-c2x*cmt5)*cmt2)/t25)/t24-(((c3x*cmt2-c2x*cmt5)*cmt5)/t25-((c4x*cmt3-c3x*cmt6)*cmt3)/t36)/t35+(cmt4*((c2x*cmt1-c1x*cmt4)/t14-(c3x*cmt2-c2x*cmt5)/t25-((c1x-c2x)*cmt4)/t14+((c2x-c3x)*cmt2)/t25))/t24-(cmt3*((c3x*cmt2-c2x*cmt5)/t25-(c4x*cmt3-c3x*cmt6)/t36-((c2x-c3x)*cmt5)/t25+((c3x-c4x)*cmt3)/t36))/t35)/(t3-t4)-((((c2x*(curt_2-t1)-c1x*(curt_2-t4))*(curt_2-t4))/t14-((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t2))/t25)/t24-(((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t5))/t25-((c4x*(curt_2-t3)-c3x*(curt_2-t6))*(curt_2-t3))/t36)/t35+((curt_2-t4)*((c2x*(curt_2-t1)-c1x*(curt_2-t4))/t14-(c3x*(curt_2-t2)-c2x*(curt_2-t5))/t25-((c1x-c2x)*(curt_2-t4))/t14+((c2x-c3x)*(curt_2-t2))/t25))/t24-((curt_2-t3)*((c3x*(curt_2-t2)-c2x*(curt_2-t5))/t25-(c4x*(curt_2-t3)-c3x*(curt_2-t6))/t36-((c2x-c3x)*(curt_2-t5))/t25+((c3x-c4x)*(curt_2-t3))/t36))/t35)/(t3-t4))*-2.0;
dfx[svpos[1]] += ((((cmt1*cmt4)/t14+(cmt2*cmt5)/t25)/t24+(cmt4*(cmt1/t14+cmt2/t25+cmt4/t14+cmt5/t25))/t24+pow(curt-t5,2.0)/(t25*t35)+(cmt3*cmt5*2.0)/(t25*t35))/(t3-t4)-((((curt_2-t1)*(curt_2-t4))/t14+((curt_2-t2)*(curt_2-t5))/t25)/t24+((curt_2-t4)*((curt_2-t1)/t14+(curt_2-t2)/t25+(curt_2-t4)/t14+(curt_2-t5)/t25))/t24+pow(curt_2-t5,2.0)/(t25*t35)+((curt_2-t3)*(curt_2-t5)*2.0)/(t25*t35))/(t3-t4))*(((((c2x*cmt1-c1x*cmt4)*cmt4)/t14-((c3x*cmt2-c2x*cmt5)*cmt2)/t25)/t24-(((c3x*cmt2-c2x*cmt5)*cmt5)/t25-((c4x*cmt3-c3x*cmt6)*cmt3)/t36)/t35+(cmt4*((c2x*cmt1-c1x*cmt4)/t14-(c3x*cmt2-c2x*cmt5)/t25-((c1x-c2x)*cmt4)/t14+((c2x-c3x)*cmt2)/t25))/t24-(cmt3*((c3x*cmt2-c2x*cmt5)/t25-(c4x*cmt3-c3x*cmt6)/t36-((c2x-c3x)*cmt5)/t25+((c3x-c4x)*cmt3)/t36))/t35)/(t3-t4)-((((c2x*(curt_2-t1)-c1x*(curt_2-t4))*(curt_2-t4))/t14-((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t2))/t25)/t24-(((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t5))/t25-((c4x*(curt_2-t3)-c3x*(curt_2-t6))*(curt_2-t3))/t36)/t35+((curt_2-t4)*((c2x*(curt_2-t1)-c1x*(curt_2-t4))/t14-(c3x*(curt_2-t2)-c2x*(curt_2-t5))/t25-((c1x-c2x)*(curt_2-t4))/t14+((c2x-c3x)*(curt_2-t2))/t25))/t24-((curt_2-t3)*((c3x*(curt_2-t2)-c2x*(curt_2-t5))/t25-(c4x*(curt_2-t3)-c3x*(curt_2-t6))/t36-((c2x-c3x)*(curt_2-t5))/t25+((c3x-c4x)*(curt_2-t3))/t36))/t35)/(t3-t4))*2.0;
dfx[svpos[2]] += ((((cmt2*cmt5)/t25+(cmt3*cmt6)/t36)/t35+(cmt3*(cmt2/t25+cmt3/t36+cmt5/t25+cmt6/t36))/t35+pow(curt-t2,2.0)/(t24*t25)+(cmt2*cmt4*2.0)/(t24*t25))/(t3-t4)-((((curt_2-t2)*(curt_2-t5))/t25+((curt_2-t3)*(curt_2-t6))/t36)/t35+((curt_2-t3)*((curt_2-t2)/t25+(curt_2-t3)/t36+(curt_2-t5)/t25+(curt_2-t6)/t36))/t35+pow(curt_2-t2,2.0)/(t24*t25)+((curt_2-t2)*(curt_2-t4)*2.0)/(t24*t25))/(t3-t4))*(((((c2x*cmt1-c1x*cmt4)*cmt4)/t14-((c3x*cmt2-c2x*cmt5)*cmt2)/t25)/t24-(((c3x*cmt2-c2x*cmt5)*cmt5)/t25-((c4x*cmt3-c3x*cmt6)*cmt3)/t36)/t35+(cmt4*((c2x*cmt1-c1x*cmt4)/t14-(c3x*cmt2-c2x*cmt5)/t25-((c1x-c2x)*cmt4)/t14+((c2x-c3x)*cmt2)/t25))/t24-(cmt3*((c3x*cmt2-c2x*cmt5)/t25-(c4x*cmt3-c3x*cmt6)/t36-((c2x-c3x)*cmt5)/t25+((c3x-c4x)*cmt3)/t36))/t35)/(t3-t4)-((((c2x*(curt_2-t1)-c1x*(curt_2-t4))*(curt_2-t4))/t14-((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t2))/t25)/t24-(((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t5))/t25-((c4x*(curt_2-t3)-c3x*(curt_2-t6))*(curt_2-t3))/t36)/t35+((curt_2-t4)*((c2x*(curt_2-t1)-c1x*(curt_2-t4))/t14-(c3x*(curt_2-t2)-c2x*(curt_2-t5))/t25-((c1x-c2x)*(curt_2-t4))/t14+((c2x-c3x)*(curt_2-t2))/t25))/t24-((curt_2-t3)*((c3x*(curt_2-t2)-c2x*(curt_2-t5))/t25-(c4x*(curt_2-t3)-c3x*(curt_2-t6))/t36-((c2x-c3x)*(curt_2-t5))/t25+((c3x-c4x)*(curt_2-t3))/t36))/t35)/(t3-t4))*-2.0;
dfx[svpos[3]] += ((pow(curt-t3,2.0)*3.0)/((t3-t4)*t35*t36)-(pow(curt_2-t3,2.0)*3.0)/((t3-t4)*t35*t36))*(((((c2x*cmt1-c1x*cmt4)*cmt4)/t14-((c3x*cmt2-c2x*cmt5)*cmt2)/t25)/t24-(((c3x*cmt2-c2x*cmt5)*cmt5)/t25-((c4x*cmt3-c3x*cmt6)*cmt3)/t36)/t35+(cmt4*((c2x*cmt1-c1x*cmt4)/t14-(c3x*cmt2-c2x*cmt5)/t25-((c1x-c2x)*cmt4)/t14+((c2x-c3x)*cmt2)/t25))/t24-(cmt3*((c3x*cmt2-c2x*cmt5)/t25-(c4x*cmt3-c3x*cmt6)/t36-((c2x-c3x)*cmt5)/t25+((c3x-c4x)*cmt3)/t36))/t35)/(t3-t4)-((((c2x*(curt_2-t1)-c1x*(curt_2-t4))*(curt_2-t4))/t14-((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t2))/t25)/t24-(((c3x*(curt_2-t2)-c2x*(curt_2-t5))*(curt_2-t5))/t25-((c4x*(curt_2-t3)-c3x*(curt_2-t6))*(curt_2-t3))/t36)/t35+((curt_2-t4)*((c2x*(curt_2-t1)-c1x*(curt_2-t4))/t14-(c3x*(curt_2-t2)-c2x*(curt_2-t5))/t25-((c1x-c2x)*(curt_2-t4))/t14+((c2x-c3x)*(curt_2-t2))/t25))/t24-((curt_2-t3)*((c3x*(curt_2-t2)-c2x*(curt_2-t5))/t25-(c4x*(curt_2-t3)-c3x*(curt_2-t6))/t36-((c2x-c3x)*(curt_2-t5))/t25+((c3x-c4x)*(curt_2-t3))/t36))/t35)/(t3-t4))*2.0;
dfx[svpos[4]] += ((pow(curt-t4,2.0)*3.0)/(t14*t24*(t3-t4))-(pow(curt_2-t4,2.0)*3.0)/(t14*t24*(t3-t4)))*(((((c2y*cmt1-c1y*cmt4)*cmt4)/t14-((c3y*cmt2-c2y*cmt5)*cmt2)/t25)/t24-(((c3y*cmt2-c2y*cmt5)*cmt5)/t25-((c4y*cmt3-c3y*cmt6)*cmt3)/t36)/t35+(cmt4*((c2y*cmt1-c1y*cmt4)/t14-(c3y*cmt2-c2y*cmt5)/t25-((c1y-c2y)*cmt4)/t14+((c2y-c3y)*cmt2)/t25))/t24-(cmt3*((c3y*cmt2-c2y*cmt5)/t25-(c4y*cmt3-c3y*cmt6)/t36-((c2y-c3y)*cmt5)/t25+((c3y-c4y)*cmt3)/t36))/t35)/(t3-t4)-((((c2y*(curt_2-t1)-c1y*(curt_2-t4))*(curt_2-t4))/t14-((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t2))/t25)/t24-(((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t5))/t25-((c4y*(curt_2-t3)-c3y*(curt_2-t6))*(curt_2-t3))/t36)/t35+((curt_2-t4)*((c2y*(curt_2-t1)-c1y*(curt_2-t4))/t14-(c3y*(curt_2-t2)-c2y*(curt_2-t5))/t25-((c1y-c2y)*(curt_2-t4))/t14+((c2y-c3y)*(curt_2-t2))/t25))/t24-((curt_2-t3)*((c3y*(curt_2-t2)-c2y*(curt_2-t5))/t25-(c4y*(curt_2-t3)-c3y*(curt_2-t6))/t36-((c2y-c3y)*(curt_2-t5))/t25+((c3y-c4y)*(curt_2-t3))/t36))/t35)/(t3-t4))*-2.0;
dfx[svpos[5]] += ((((cmt1*cmt4)/t14+(cmt2*cmt5)/t25)/t24+(cmt4*(cmt1/t14+cmt2/t25+cmt4/t14+cmt5/t25))/t24+pow(curt-t5,2.0)/(t25*t35)+(cmt3*cmt5*2.0)/(t25*t35))/(t3-t4)-((((curt_2-t1)*(curt_2-t4))/t14+((curt_2-t2)*(curt_2-t5))/t25)/t24+((curt_2-t4)*((curt_2-t1)/t14+(curt_2-t2)/t25+(curt_2-t4)/t14+(curt_2-t5)/t25))/t24+pow(curt_2-t5,2.0)/(t25*t35)+((curt_2-t3)*(curt_2-t5)*2.0)/(t25*t35))/(t3-t4))*(((((c2y*cmt1-c1y*cmt4)*cmt4)/t14-((c3y*cmt2-c2y*cmt5)*cmt2)/t25)/t24-(((c3y*cmt2-c2y*cmt5)*cmt5)/t25-((c4y*cmt3-c3y*cmt6)*cmt3)/t36)/t35+(cmt4*((c2y*cmt1-c1y*cmt4)/t14-(c3y*cmt2-c2y*cmt5)/t25-((c1y-c2y)*cmt4)/t14+((c2y-c3y)*cmt2)/t25))/t24-(cmt3*((c3y*cmt2-c2y*cmt5)/t25-(c4y*cmt3-c3y*cmt6)/t36-((c2y-c3y)*cmt5)/t25+((c3y-c4y)*cmt3)/t36))/t35)/(t3-t4)-((((c2y*(curt_2-t1)-c1y*(curt_2-t4))*(curt_2-t4))/t14-((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t2))/t25)/t24-(((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t5))/t25-((c4y*(curt_2-t3)-c3y*(curt_2-t6))*(curt_2-t3))/t36)/t35+((curt_2-t4)*((c2y*(curt_2-t1)-c1y*(curt_2-t4))/t14-(c3y*(curt_2-t2)-c2y*(curt_2-t5))/t25-((c1y-c2y)*(curt_2-t4))/t14+((c2y-c3y)*(curt_2-t2))/t25))/t24-((curt_2-t3)*((c3y*(curt_2-t2)-c2y*(curt_2-t5))/t25-(c4y*(curt_2-t3)-c3y*(curt_2-t6))/t36-((c2y-c3y)*(curt_2-t5))/t25+((c3y-c4y)*(curt_2-t3))/t36))/t35)/(t3-t4))*2.0;
dfx[svpos[6]] += ((((cmt2*cmt5)/t25+(cmt3*cmt6)/t36)/t35+(cmt3*(cmt2/t25+cmt3/t36+cmt5/t25+cmt6/t36))/t35+pow(curt-t2,2.0)/(t24*t25)+(cmt2*cmt4*2.0)/(t24*t25))/(t3-t4)-((((curt_2-t2)*(curt_2-t5))/t25+((curt_2-t3)*(curt_2-t6))/t36)/t35+((curt_2-t3)*((curt_2-t2)/t25+(curt_2-t3)/t36+(curt_2-t5)/t25+(curt_2-t6)/t36))/t35+pow(curt_2-t2,2.0)/(t24*t25)+((curt_2-t2)*(curt_2-t4)*2.0)/(t24*t25))/(t3-t4))*(((((c2y*cmt1-c1y*cmt4)*cmt4)/t14-((c3y*cmt2-c2y*cmt5)*cmt2)/t25)/t24-(((c3y*cmt2-c2y*cmt5)*cmt5)/t25-((c4y*cmt3-c3y*cmt6)*cmt3)/t36)/t35+(cmt4*((c2y*cmt1-c1y*cmt4)/t14-(c3y*cmt2-c2y*cmt5)/t25-((c1y-c2y)*cmt4)/t14+((c2y-c3y)*cmt2)/t25))/t24-(cmt3*((c3y*cmt2-c2y*cmt5)/t25-(c4y*cmt3-c3y*cmt6)/t36-((c2y-c3y)*cmt5)/t25+((c3y-c4y)*cmt3)/t36))/t35)/(t3-t4)-((((c2y*(curt_2-t1)-c1y*(curt_2-t4))*(curt_2-t4))/t14-((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t2))/t25)/t24-(((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t5))/t25-((c4y*(curt_2-t3)-c3y*(curt_2-t6))*(curt_2-t3))/t36)/t35+((curt_2-t4)*((c2y*(curt_2-t1)-c1y*(curt_2-t4))/t14-(c3y*(curt_2-t2)-c2y*(curt_2-t5))/t25-((c1y-c2y)*(curt_2-t4))/t14+((c2y-c3y)*(curt_2-t2))/t25))/t24-((curt_2-t3)*((c3y*(curt_2-t2)-c2y*(curt_2-t5))/t25-(c4y*(curt_2-t3)-c3y*(curt_2-t6))/t36-((c2y-c3y)*(curt_2-t5))/t25+((c3y-c4y)*(curt_2-t3))/t36))/t35)/(t3-t4))*-2.0;
dfx[svpos[7]] += ((pow(curt-t3,2.0)*3.0)/((t3-t4)*t35*t36)-(pow(curt_2-t3,2.0)*3.0)/((t3-t4)*t35*t36))*(((((c2y*cmt1-c1y*cmt4)*cmt4)/t14-((c3y*cmt2-c2y*cmt5)*cmt2)/t25)/t24-(((c3y*cmt2-c2y*cmt5)*cmt5)/t25-((c4y*cmt3-c3y*cmt6)*cmt3)/t36)/t35+(cmt4*((c2y*cmt1-c1y*cmt4)/t14-(c3y*cmt2-c2y*cmt5)/t25-((c1y-c2y)*cmt4)/t14+((c2y-c3y)*cmt2)/t25))/t24-(cmt3*((c3y*cmt2-c2y*cmt5)/t25-(c4y*cmt3-c3y*cmt6)/t36-((c2y-c3y)*cmt5)/t25+((c3y-c4y)*cmt3)/t36))/t35)/(t3-t4)-((((c2y*(curt_2-t1)-c1y*(curt_2-t4))*(curt_2-t4))/t14-((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t2))/t25)/t24-(((c3y*(curt_2-t2)-c2y*(curt_2-t5))*(curt_2-t5))/t25-((c4y*(curt_2-t3)-c3y*(curt_2-t6))*(curt_2-t3))/t36)/t35+((curt_2-t4)*((c2y*(curt_2-t1)-c1y*(curt_2-t4))/t14-(c3y*(curt_2-t2)-c2y*(curt_2-t5))/t25-((c1y-c2y)*(curt_2-t4))/t14+((c2y-c3y)*(curt_2-t2))/t25))/t24-((curt_2-t3)*((c3y*(curt_2-t2)-c2y*(curt_2-t5))/t25-(c4y*(curt_2-t3)-c3y*(curt_2-t6))/t36-((c2y-c3y)*(curt_2-t5))/t25+((c3y-c4y)*(curt_2-t3))/t36))/t35)/(t3-t4))*2.0;

	    
	    
	    }
             
              cnt++;
        }
        splos += splpieces*8;
        knoffset += (splpieces+7);
        indexoffset += cnt;
    }
    
    
 free(svpos);   
}