#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define N 5000
#define GRAN (2*pow(N*10,2))
int X[N];
int Y[N];
float distancia[N][N+1];

int cami[N],bo[N];

int main(int na, char* arg[])
{
int nn,i,j,primer,actual,index;
float dmin,dist,millor;

    assert(na == 2);
    printf("Dimensio %s\n",arg[1]);
    nn=atoi(arg[1]);
    assert(nn<=N);

    for(i=0; i<nn; i++) X[i]=rand()%(nn*10);
    for(i=0; i<nn; i++) Y[i]=rand()%(nn*10);
  
    for(i=0; i<nn; i++) distancia[i][i]=0;

    for(i=0; i<nn; i++)  
        for(j=i+1; j<nn; j++) 
            distancia[i][j]= distancia[j][i] = sqrt(pow(X[i]-X[j],2) + pow(Y[i]-Y[j],2));

// TOTs amb Greedy

    millor = GRAN;
    for (primer=0; primer<nn; primer++) {
        dist = 0;
        for(i=0;i<nn;i++) cami[i]=-1;
        cami[primer]=0;
        actual = primer;
        for (i=1; i<nn; i++) {
            dmin = GRAN;
            index=0; // redundant
            for (j=0; j<nn; j++) {
                if (cami[j]==-1 && actual!=j && distancia[actual][j] < dmin) {
                    dmin = distancia[actual][j];
                    index= j;
                }
            }
            actual = index;
            cami[actual] = i;
            dist += dmin;
            // PODA
            if (dist >= millor) { dist = 0; break;}
        }
        if (dist) {
            dmin = distancia[actual][primer];
            dist += dmin;
            if (dist < millor) {
                for(i=0;i<nn;i++) bo[cami[i]]=i;
                millor = dist;
            }
            distancia[primer][nn]=dist;  // per guardar alternatives
        }
    }
    
    printf("Solucio :\n");
    for(i=0; i<nn; i++) printf("%d\n",bo[i]);
    printf ("Distancia %g == %g\n",millor,distancia[bo[0]][nn]);

exit(0);
}
