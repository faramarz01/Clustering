"""
Program Name: Clustring.py
Author:       Dr. Faramarz Azadegan 
Version:      0.0
Date:         June 11, 2019
Contact Info: faramarz_azadegan@yahoo.com
################
# TotalCG:       Number of clusters or "center of gravity"
# cg[]:          center of gravity for each cluster
# cg_count[]:    count of vectors in each cluster
# TotalDist[]:   total distortion in each iteration
# Dist_class[]:  distorion in each cluster
# Vector[]:      vectors of dimension Dim
# ClassAssgn[]:  class assignment for each vector
################
"""
Dim=8                     # dimention of vectors
dim=Dim
INFY=Dim*255*255          # Infinity in this program
#PERTURB=1.1               # Purturbation for cluster splitting
PERTURB=0.9
Perturb=PERTURB
EPSILON=0.005             # Stopping threshold for iteration
Samples=720*8             # Number of samples for this example
NO_VECTORS=Samples/Dim    # Number of vectors
Max_cg=32                # Maximum number of clusters in this example
Verbose=0                # Do not print the values if zero
MaxIter=40               # Maximum number of iteration
#-------------------------------------------
# Use the pixels in an image to form vectors for input
fo=open("/home/faramarz/Images/mobile.y", "rb")
Vector=[0 for i in range(Samples)]
for i in range(Samples):
    t=fo.read(1)
    Vector[i]=ord(t)

if Verbose: print("Calculate CG")
no_cg = 1                  # Start with one cluster
TotalCG=NO_VECTORS         # Maximum number of clusters is total number of vectors
sum=[]                     # Contains the sum over all the vectors
sum=[0 for i in range(Dim)] # Initialize to zero
cg=[]                        # center of gravity
cg=[0 for i in range(int(Dim*TotalCG))] # initialize to zero
NoVectors=NO_VECTORS
NO_VECTORS=int(Samples/Dim)
NoVectors=NO_VECTORS
for k in range(Dim):                  # calculate sum of all vectors
    for i in range(NoVectors): sum[k]+=Vector[i*Dim+k]
    sum[k]/=NoVectors                  # fine center of gravity of all vectors
    cg[k]=sum[k]
if Verbose: print("split into two")          
index=no_cg*Dim
Perturb=PERTURB
for g in range(no_cg*Dim): cg[index+g]=cg[g]*Perturb # split in two 
no_cg*=2
if Verbose:
    for g in range(no_cg*Dim): print(cg[g])
TotalDist=[]
MaxClass=NoVectors
TotalDist=[0 for i in range(MaxIter)]    # initiallize distortion
Dist_class=[]
Dist_class=[0 for i in range(MaxClass)]  # initiallize distortion per class
ClassAssgn=[]
ClassAssgn=[0 for i in range(NoVectors)] # initiallize class assignment for vectors
iter=0
for vect in range(NoVectors):           # make class assignment for each vector 
    min=INFY
    for c in range(no_cg):
        dist=0
        for d in range(dim):
            temp=Vector[vect*dim+d]-cg[c*dim+d]      # L2 norm
            dist+=(temp*temp)
        if(dist<min):                                # find the best match
            min=dist
            index=c
        Dist_class[index]+=min                       # sum up the distortion for a class
        TotalDist[iter]+=min                         # total distortion for this iteration
        ClassAssgn[vect]=index                       # cluster assignment for this vector
cg_count=[]                                          # count of vectors for each cluster
cg_count=[0 for i in range(Max_cg)]   # initialize the count to zero for next iteration
#for Class in range(Max_cg):
#    print(cg_count[Class])
while no_cg <= Max_cg:                               # so long as smaller than max cg
    ratio=1.
    for i in range(Max_cg): cg_count[i]=0   # initialize to zero       
    if Verbose: print("Start: no_cg = ", no_cg)
    iter=0                                           # initialize the iteration
    while ratio>EPSILON:                            # so long as the ratio is big
        TotalDist[iter]=0.                 
        Dist_class=[0 for i in range(MaxClass)]
        for vect in range(NoVectors):                # same procedure as above
            min=INFY
            for c in range(no_cg):
                dist=0
                for d in range(dim):
                    temp=Vector[vect*dim+d]-cg[c*dim+d]   # use L2 norm
                    dist+=(temp*temp)
                if(dist<min):
                    min=dist
                    index=c
            Dist_class[index]+=min
            TotalDist[iter]+=min
            ClassAssgn[vect]=index       
        for i in range(dim*no_cg): cg[i]=0           # reset the centroids to zero
        for i in range(Max_cg): cg_count[i]=0       # set count of vectors in each cluster 
        for vect in range(NoVectors):                     # examine all vectors
                Vect_class=ClassAssgn[vect]               # based on the assignment
                for k in range(dim): cg[Vect_class*dim+k]+=Vector[vect*dim+k]    # sum up the vectors
                cg_count[Vect_class]+=1                     # increase the count
        if Verbose: print("before", cg_count)
        for Class in range(no_cg):                          # find cg's
                for k in range(dim):
#                       if Verbose: print("Class ", cg_count[Class])
                        if(cg_count[Class]!=0): cg[Class*dim+k]/=cg_count[Class]
        if Verbose:
            print("============================================")
            print("normalized, iter=", iter)
            for i in range(no_cg*dim): print(cg[i])
        if(iter>0):
            Diff=TotalDist[iter-1]-TotalDist[iter]  # diff of two consecutive distortions
            ratio=Diff/TotalDist[iter-1]            # ratio of diff to previous distortions
            if Verbose:
                print("ratio=", ratio)
                print("Distortion=", TotalDist[iter])
        iter+=1
        if Verbose: print("iter=", iter)
    #print("end")
    for Class in range(0,no_cg,1): print("no_cg ", no_cg, "Class ",Class, cg_count[Class]) 
    index=no_cg*Dim
    no_cg*=2                                         # split the cg's into two
    for g in range(no_cg*Dim):
        cg[index+g]=cg[g]*Perturb                   # value assignment for new cg's
        if Verbose: print(cg[index+g])
    if Verbose: print("no_cg = ", no_cg)   
  




                
        
                
        


        

