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

int main(int na, char* arg[])
{
    int nn,i,j,primer,actual,index,end;
    int origin = 0;
    float dmin,dist,millor = GRAN;
    float aux = GRAN;

    int el_meu_rank; // rank
    int p; // num_processos
    MPI_Status estat; // estat de la recepció
    
    assert(na == 2);

    MPI_Init(&na, &arg); // Init MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &el_meu_rank); // Obtenir rank del proces
    MPI_Comm_size(MPI_COMM_WORLD, &p); // Obtenir num_processos

    float millors[p];
    int millor_rank = el_meu_rank; // millor rank
    int sizes[p]; 

    nn = atoi(arg[1]);
    assert(nn<=N);
    srand(1);
    
    int buff_size = nn / p;
    int extra_size = nn % p;

    for (i = 0; i < p; ++i)
        sizes[i] = buff_size;
    i = 0;
    while (extra_size > 0)
    {
        sizes[i++] += 1;
        extra_size--;
    }
    for (i = 1; i < p; ++i)
        sizes[i] += sizes[i-1];

    if (el_meu_rank != 0)
        origin = sizes[el_meu_rank - 1];

    end = sizes[el_meu_rank];

    for(i=0; i<nn; i++) X[i]=rand()%(nn*10);
    for(i=0; i<nn; i++) Y[i]=rand()%(nn*10);
    // for(i=0; i<nn; i++) distancia[i][i]=0;
    for(i=0; i<nn; i++)  
        for(j=i+1; j<nn; j++) 
            distancia[i][j]= distancia[j][i] = sqrt(pow(X[i]-X[j],2) + pow(Y[i]-Y[j],2));
    
    // TOTs amb Greedy
    for (primer = origin; primer < end; primer++) 
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
            if (dist >= millor) { dist = 0; break;}
        }
        if (dist) 
        {
            dmin = distancia[actual][primer];
            dist += dmin;
            if (dist < millor) 
            {
                for(i = 0; i < nn; i++) bo[cami[i]] = i;
                millor = dist;
            } 
            distancia[primer][nn] = dist;  // per guardar alternatives
        }  
    }

    MPI_Gather(&millor, 1, MPI_FLOAT, millors, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (el_meu_rank == 0)
    {
       for(i = 0; i < p; i++)
       {
            if (millors[i] < aux)
            {
                millor_rank = i;
                aux = millors[i];
            }
       }
    }
    
    MPI_Bcast(&millor_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (el_meu_rank == millor_rank && el_meu_rank != 0)
    {
        MPI_Send(bo, nn, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&distancia[bo[0]][nn], 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }

    if (el_meu_rank == 0)
    {
        millor = millors[millor_rank];
        if (millor_rank != 0)
        {
            MPI_Recv(bo, nn, MPI_INT, millor_rank, 0, MPI_COMM_WORLD, &estat);
            MPI_Recv(&distancia[bo[0]][nn], 1, MPI_FLOAT, millor_rank, 0, MPI_COMM_WORLD, &estat);
        }
        printf("Dimensio %s\n",arg[1]);
        printf("Solucio :\n");
        for(i = 0; i < nn; i++)
            printf("%d\n",bo[i]);
        printf("Distancia %g == %g\n", millor, distancia[bo[0]][nn]);
    }

    MPI_Finalize();
    exit(0);
}
