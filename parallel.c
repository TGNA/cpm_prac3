#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>

#define N 5000
#define GRAN (2*pow(N*10,2))
#define MSGLEN 100
int X[N];
int Y[N];
float distancia[N][N+1];

int cami[N],bo[N];

void stuff() 
{

}

int main(int na, char* arg[]){
    int nn,i,j,primer,actual,index;
    float dmin,dist,millor = GRAN;

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

    int el_meu_rank; // rank
    int p; // num_processos
    int font; // rank emissor
    int desti = 0; // rank receptor
    int etiq = 0; // etiqueta missatge
    MPI_Status estat; // estat de la recepció

    MPI_Init(&num_args, &args); // Init MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &el_meu_rank); // Obtenir rank del proces
    MPI_Comm_size(MPI_COMM_WORLD, &p); // Obtenir num_processos

    int buffSize = N / p;
    int extraSize = N % p;

    // TOTs amb Greedy
    for (primer=el_meu_rank*buffSize; primer<(el_meu_rank+1)*buffSize; primer++) 
    {
        dist = 0;
        for(i=el_meu_rank*buffSize;i<(el_meu_rank+1)*buffSize;i++) cami[i]=-1;
        cami[primer]=0;
        // MPI_Bcast(cami, N, MPI_INT, etiq, MPI_COMM_WORLD);
        actual = primer;
        for (i=el_meu_rank*buffSize+1; i<(el_meu_rank+1)*buffSize; i++) 
        {
            dmin = GRAN;
            index=0; // redundant
            for (j=el_meu_rank*buffSize; j<(el_meu_rank+1)*buffSize; j++) 
            {
                if (cami[j]==-1 && actual!=j && distancia[actual][j] < dmin) 
                {
                    dmin = distancia[actual][j];
                    index= j;
                }
            }
            actual = index;
            cami[actual] = i;
            dist += dmin;
            // PODA
            MPI_Gather(millor, 1, MPI_INT, millor, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if (dist >= millor) { dist = 0; break;}
        }
        if (dist) 
        {
            dmin = distancia[actual][primer];
            dist += dmin;
            if (dist < millor) 
            {
                for(i=el_meu_rank*buffSize;i<(el_meu_rank+1)*buffSize;i++) bo[cami[i]]=i;
                millor = dist;
                MPI_Bcast(millor, 1, MPI_INT, etiq, MPI_COMM_WORLD);
            }
            distancia[primer][nn]=dist;  // per guardar alternatives
        }
    }


    if(el_meu_rank == 0)
    {
        for (font = 1; i < p; i++)
        {
            MPI_Recv(missatge, MSGLEN + 1, MPI_CHAR, font, etiq, MPI_COMM_WORLD, &estat);
            printf("%s\n", missatge);
        }
        printf("Solucio :\n");
        for(i=0; i<nn; i++)
            printf("%d\n",bo[i]);
        printf("Distancia %g == %g\n", millor, distancia[bo[0]][nn]);
    }
    MPI_Finalize();
    exit(0);
}
