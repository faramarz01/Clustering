#include <stdio.h>
//#define DIMENSION  2
//#define DIMENSION  4
#define DIMENSION  2
#define NO_ROWS    480
#define NO_COLS    720
#define NO_CG      8
#define INFY       10000000
#define PERTURB    0.1
#define EPSILON    0.05
struct vector{
  float dimension[DIMENSION];
  int class_assignment;
} Vect[NO_ROWS*NO_COLS/DIMENSION];
struct CENTER_GRAVITY{
  float dimension[DIMENSION];
  int count;
}cg[NO_CG];
  
void main()
{
  unsigned char img[NO_ROWS][NO_COLS], arr[NO_COLS];
  FILE *fp;
  int row, col, k, i, c, vectors=0;
  int NO_VECTORS, no_cg, vect;
  int min, dist, index, class, min_dist, iter;
  float  sum[DIMENSION], TotalDist[20], ratio, temp, Diff, Dist_class[NO_CG];
  // **************************************************
  // open image for reading
  // **************************************************  
  fp=fopen("mobile.y", "rb");
  for(row=0;row<NO_ROWS; row++){
    fread(arr,NO_COLS, sizeof(unsigned char), fp);
    for(col=0;col<NO_COLS;col++)img[row][col]=arr[col];
  }
  // **************************************************
  // read image and create vectors of size DIMENSION
  // **************************************************  
  printf("read image\n");
  for(row=0;row<NO_ROWS; row++){
    //  printf("row = %d\n", row);
    for(col=0;col<NO_COLS; col+=DIMENSION){
      for(i=0;i<DIMENSION;i++)
	Vect[vectors].dimension[i]=img[row][col+i];
      vectors++;
    }
  }
  NO_VECTORS=vectors;
  // printf("vectors: %d, %d\n", vectors, NO_VECTORS);
  // **************************************************
  // Calculate center of gravity of all vectors
  // **************************************************
  printf("Calculate CG\n");
  no_cg=1;
  for(k=0;k<DIMENSION;k++){
    sum[k]=0;
    for(i=0;i<NO_VECTORS;i++)
      sum[k]+=Vect[i].dimension[k];
    sum[k]/=NO_VECTORS;
    cg[0].dimension[k]=sum[k];
    printf("sum[%d]=%f\n", k, sum[k]);
  }
  // **************************************************
  // Split cg into two
  // **************************************************
  //  printf("split into two\n");
    printf("split into three\n");
  for(c=0;c<no_cg;c++) {
    for(k=0;k<DIMENSION;k++){
      cg[no_cg+c].dimension[k]=cg[c].dimension[k]*(1.+PERTURB);
      cg[2+c].dimension[k]=cg[c].dimension[k]*(1.-PERTURB);
      //      printf("%f, %f \t",cg[c].dimension[k], cg[no_cg+c].dimension[k]);
      printf("%f, %f, %f \t",cg[c].dimension[k], cg[no_cg+c].dimension[k],
	     cg[2+c].dimension[k]);
    }
    printf("\n");
  }
  //no_cg*=2;
  no_cg*=3;
  // **************************************************
  // Iterate until you reach a local min
  // **************************************************
  printf("start iterating\n");
  iter=0;
  ratio=1;
  while(ratio > EPSILON){
    printf("************** iter = %d ******************\n", iter);
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
	  dist+=(temp*temp);  // distortion
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
    for(c=0; c<no_cg; c++) printf("Dist_class[%d]=%f\n", c, Dist_class[c]);
    printf("TotalDist[%d]=%f, normalized: %f\n", iter, TotalDist[iter],
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
    //printf("no of vectors %d\n", NO_VECTORS);
    for(vect=0; vect<NO_VECTORS;vect++){
      class=Vect[vect].class_assignment;
      for(k=0;k<DIMENSION;k++)
	cg[class].dimension[k]+=Vect[vect].dimension[k];
      cg[class].count++;
    }
    
    vect=0;
    for(c=0;c<no_cg;c++){
      printf("cg[%d]=%d\t", c, cg[c].count);
      vect+=cg[c].count;
    }
    printf("total vectors: %d\n", vect);
    
    // ************************************************************************
    // normalize the cg
    // ************************************************************************
    //    printf("normalize cg\n");
    for(class=0;class<no_cg;class++){
      for(k=0;k<DIMENSION;k++){
	cg[class].dimension[k]/=cg[class].count;
	printf("%f, ", cg[class].dimension[k]);
      }
      printf("\n");
      //printf("count[%d]=%d \n", class, cg[class].count);
    }
    // printf("dist[%d]: %f\n", iter, TotalDist[iter]);
    printf(" ------------------------------\n");
    if(iter>0){
      printf("TotalDist[%d]=%f, TotalDist[%d]=%f\n", iter-1,TotalDist[iter-1],
	     iter, TotalDist[iter]);
      Diff=TotalDist[iter-1]-TotalDist[iter]; printf("Diff=%f\n",Diff);
      ratio=(TotalDist[iter-1]-TotalDist[iter])/TotalDist[iter-1];
      printf("ratio=%f\n", ratio);
    }
    iter++; TotalDist[iter]=0.;
  }
  // ************************************************************************
  return;
}
