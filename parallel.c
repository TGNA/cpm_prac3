#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>

#define N 5000
#define GRAN (2*pow(N*10,2))

int X[N];
int Y[N];
float distancia[N][N+1];

int cami[N],bo[N];

int main(int na, char* arg[]){
    int nn,i,j,primer,actual,index;
    float dmin,dist,millor = GRAN;
    float aux = -1;

    assert(na == 2);

    int el_meu_rank; // rank
    int p; // num_processos
    int font; // rank emissor
    int millor_rank; // millor rank
    MPI_Status estat; // estat de la recepció

    MPI_Init(&na, &arg); // Init MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &el_meu_rank); // Obtenir rank del proces
    MPI_Comm_size(MPI_COMM_WORLD, &p); // Obtenir num_processos

    millor_rank = -1;

    nn = atoi(arg[1]);
    assert(nn<=N);
    srand(1);
    if(el_meu_rank == 0)
    {
        printf("Dimensio %s\n",arg[1]);

        for(i=0; i<nn; i++) X[i]=rand()%(nn*10);
        for(i=0; i<nn; i++) Y[i]=rand()%(nn*10);

        for(i=0; i<nn; i++) distancia[i][i]=0;
        for(i=0; i<nn; i++)  
            for(j=i+1; j<nn; j++) 
                distancia[i][j]= distancia[j][i] = sqrt(pow(X[i]-X[j],2) + pow(Y[i]-Y[j],2));
    }

    MPI_Bcast(&distancia, N*(N+1), MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    int buffSize = nn / p;
    int extraSize = nn % p;

    // TOTs amb Greedy
    for (primer=el_meu_rank*buffSize; primer<(el_meu_rank+1)*buffSize; primer++) 
    {
        dist = 0;
        for(i = 0; i < nn; i++) cami[i] = -1;
        cami[primer] = 0;
        actual = primer;
        for (i = 1; i < nn; i++) 
        {
            dmin = GRAN;
            index = 0; // redundant
            for (j = 0;j < nn; j++) 
            {
                if (cami[j] == -1 && actual != j && distancia[actual][j] < dmin) 
                {
                    dmin = distancia[actual][j];
                    index= j;
                }
            }
            actual = index;
            cami[actual] = i;
            dist += dmin;
            // PODA
            //if (dist >= millor) { dist = 0; break;}
        }
        for (j = 0; j < p; j++) 
        {
            if (el_meu_rank == j)
            {
                if (dist) 
                {
                    dmin = distancia[actual][primer];
                    dist += dmin;
                    if (dist < millor) 
                    {
                        for(i = 0; i < nn; i++) bo[cami[i]] = i;
                        millor = dist;
                        aux = millor;
                    } else
                        aux = -1;
                    distancia[primer][nn] = dist;  // per guardar alternatives
                }
            }
            MPI_Bcast(&aux, 1, MPI_FLOAT, j, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            if (aux <= millor && aux != -1) 
            {
                millor = aux;
                millor_rank = j;
            } 
        }
    }
    if (el_meu_rank != 0 && el_meu_rank == millor_rank) 
    {
        MPI_Send(&bo, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&distancia, N*(N+1), MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }

    if(el_meu_rank == 0)
    {
        if (el_meu_rank != millor_rank) {
            MPI_Recv(&bo, N, MPI_INT, millor_rank, 0, MPI_COMM_WORLD, &estat);
            MPI_Recv(&distancia, N*(N+1), MPI_FLOAT, millor_rank, 0, MPI_COMM_WORLD, &estat);
        }
        printf("Solucio :\n");
        for(i=0; i<nn; i++)
            printf("%d\n",bo[i]);
        printf("Distancia %g == %g\n", millor, distancia[bo[0]][nn]);
    }
    MPI_Finalize();
    exit(0);
}