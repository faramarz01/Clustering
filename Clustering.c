/* **************************************************
Program Name: Clustring.c
Author:       Dr. Faramarz Azadegan 
Version:      0.0
Date:         June 11, 2019
Contact Info: faramarz_azadegan@yahoo.com
Software available at: https://github.com/faramarz01/Clustering
************************************************** */
#include <stdio.h>
#define DIMENSION  2			// dimention of vectors
#define NO_ROWS    480			// number of rows in the image
#define NO_COLS    720			// number of columns in the image
#define NO_CG      8			// Maximum number of clusters in this
#define INFY       10000000		// Infinity in this program
#define PERTURB    0.1			// Purturbation for cluster splitting
#define EPSILON    0.05			// Stopping threshold for iteration
#define Verbose		0			// Do not print the values if zero
// ***************************************************************
struct vector{							// vectors
  float dimension[DIMENSION];
  int class_assignment;
} Vect[NO_ROWS*NO_COLS/DIMENSION];
// --------------------------------------------
struct CENTER_GRAVITY{					// center of gravity
  float dimension[DIMENSION];
  int count;
}cg[NO_CG];
  
int main()
{
  unsigned char img[NO_ROWS][NO_COLS], arr[NO_COLS];
  FILE 			*fp;
  int 			row, col, k, i, c, vectors=0;
  int 			NO_VECTORS, no_cg, vect;
  int 			min, dist, index, class, iter;
  float  		sum[DIMENSION], TotalDist[20], ratio, temp, Diff, Dist_class[NO_CG];
  // **************************************************
  // open image for reading
  // **************************************************  
  fp=fopen("mobile.y", "rb");
  if(fp==NULL){ printf("could not open mobile.y\n"); return(-1);}
  for(row=0;row<NO_ROWS; row++){
    fread(arr,NO_COLS, sizeof(unsigned char), fp);
    for(col=0;col<NO_COLS;col++)img[row][col]=arr[col];
  }
  // **************************************************
  // read image and create vectors of size DIMENSION
  // **************************************************  
  if(Verbose) printf("read image\n");
  for(row=0;row<NO_ROWS; row++){
    //  if(Verbose) printf("row = %d\n", row);
    for(col=0;col<NO_COLS; col+=DIMENSION){
      for(i=0;i<DIMENSION;i++)
		Vect[vectors].dimension[i]=img[row][col+i];
      vectors++;
    }
  }
  NO_VECTORS=vectors;
  if(Verbose)  printf("vectors: %d, %d\n", vectors, NO_VECTORS);
  // **************************************************
  // Calculate center of gravity of all vectors
  // **************************************************
  if(Verbose) printf("Calculate CG\n");
  no_cg=1;
  for(k=0;k<DIMENSION;k++){
    sum[k]=0;
    for(i=0;i<NO_VECTORS;i++)
      sum[k]+=Vect[i].dimension[k];
    sum[k]/=NO_VECTORS;
    cg[0].dimension[k]=sum[k];
    if(Verbose) printf("sum[%d]=%f\n", k, sum[k]);
  }
  // **************************************************
  // Split cg into two
  // **************************************************
  if(Verbose) printf("split into two\n");
  for(c=0;c<no_cg;c++) {
    for(k=0;k<DIMENSION;k++){
      cg[no_cg+c].dimension[k]=cg[c].dimension[k]*(1.+PERTURB);
      if(Verbose) printf("%f, %f \t",cg[c].dimension[k], cg[no_cg+c].dimension[k]);
    }
    if(Verbose) printf("\n");
  }
  no_cg*=2;				// split into two
  // **************************************************
  // Iterate until you reach a local min
  // **************************************************
  if(Verbose) printf("start iterating\n");
  iter=0;
  ratio=1;
  while(ratio > EPSILON){
    if(Verbose) printf("************** iter = %d ******************\n", iter);
    // **************************************************
    // Calculate Distortion for each centroid and assign the minimum
    // **************************************************
    TotalDist[iter]=0.; // initialize total distortion
    for(c=0; c<no_cg; c++) Dist_class[c]=0;
    for(vect=0; vect<NO_VECTORS;vect++){
		min=INFY;
		for(c=0; c<no_cg; c++){
			dist=0;
			for(k=0;k<DIMENSION;k++){ 
				temp=(Vect[vect].dimension[k]-cg[c].dimension[k]);
				dist+=(temp*temp);  // L2 distortion
			}
			if(dist<min){ 
				min=dist;   // find min
				index=c;    // save index
			}
		}
		Dist_class[index]+=min; // class distortion
		TotalDist[iter]+=min;   // update distortion
		Vect[vect].class_assignment=index;  // class assignment
	}
    if(Verbose) 
		for(c=0; c<no_cg; c++) printf("Dist_class[%d]=%f\n", c, Dist_class[c]);
    if(Verbose) printf("TotalDist[%d]=%f, normalized: %f\n", iter, TotalDist[iter],
	   TotalDist[iter]/(NO_VECTORS*DIMENSION));
    // ************************************************************************
    // calculate cg for each group
    // ************************************************************************
    for(c=0;c<no_cg;c++){
      cg[c].count=0;
      for(k=0;k<DIMENSION;k++){
		cg[c].dimension[k]=0;
      }
    }
    if(Verbose) printf("no of vectors %d\n", NO_VECTORS);
    for(vect=0; vect<NO_VECTORS;vect++){
      class=Vect[vect].class_assignment;
      for(k=0;k<DIMENSION;k++)
		cg[class].dimension[k]+=Vect[vect].dimension[k];
      cg[class].count++;
    }
    
    vect=0;
    for(c=0;c<no_cg;c++){
      if(Verbose) printf("cg[%d]=%d\t", c, cg[c].count);
      vect+=cg[c].count;
    }
    if(Verbose) printf("total vectors: %d\n", vect);
    
    // ************************************************************************
    // normalize the cg
    // ************************************************************************
    //    printf("normalize cg\n");
    for(class=0;class<no_cg;class++){
      for(k=0;k<DIMENSION;k++){
		cg[class].dimension[k]/=cg[class].count;
		if(Verbose) printf("%f, ", cg[class].dimension[k]);
      }
      if(Verbose) printf("\n");
      if(Verbose) printf("count[%d]=%d \n", class, cg[class].count);
    }
    if(Verbose) printf("dist[%d]: %f\n", iter, TotalDist[iter]);
    if(Verbose) printf(" ------------------------------\n");
    if(iter>0){
      if(Verbose) printf("TotalDist[%d]=%f, TotalDist[%d]=%f\n", iter-1,TotalDist[iter-1],
	     iter, TotalDist[iter]);
      Diff=TotalDist[iter-1]-TotalDist[iter]; if(Verbose)printf("Diff=%f\n",Diff);
      ratio=(TotalDist[iter-1]-TotalDist[iter])/TotalDist[iter-1];
      if(Verbose) printf("ratio=%f\n", ratio);
    }
    iter++; TotalDist[iter]=0.;
  }
  // ************************************************************************
  return 0;
}
