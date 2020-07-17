/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;

int main( int argc, char **argv ) {
    int rank, numprocs;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    
    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double tbeg = MPI_Wtime();

    /* write your code here */
    int local_counts[numprocs], offsets[numprocs];
    ll ue[200000],ve[200000],we[200000],dist[200000];
    ll n;
    if(rank == 0)
    {
        ifstream inp;
        inp.open(argv[1]);

        ll m;
        inp >> n >> m;
        ll j=0;
        for(int i=0;i<m;i++)
        {
            ll u,v,w;
            inp >> u >> v >> w;
            ue[j]=u;ve[j]=v;we[j]=w;j++;
            ue[j]=v;ve[j]=u;we[j]=w;j++;
        }
        ll N = j;
        ll s;
        inp >> s;
        inp.close();
        
        for(ll i=0;i<n+1;i++)
            dist[i] = LLONG_MAX;
        dist[s] = 0;   

        ll remainder = N % numprocs,sum = 0;
        for (int i = 0; i < numprocs; i++) 
        {
            local_counts[i] = N / numprocs;
            if (remainder > 0) {
                local_counts[i] += 1;
                remainder--;
            }
            offsets[i] = sum;
            sum += local_counts[i];
        }
    }
    MPI_Barrier( MPI_COMM_WORLD );

    MPI_Bcast(&local_counts,numprocs,MPI_INT,0,MPI_COMM_WORLD); 
    MPI_Bcast(&offsets,numprocs,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&n,1,MPI_LONG_LONG_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&dist,n+1,MPI_LONG_LONG_INT,0,MPI_COMM_WORLD);

    ll lue[local_counts[rank]],lve[local_counts[rank]],lwe[local_counts[rank]];
    MPI_Scatterv(&ue, local_counts, offsets,MPI_LONG_LONG_INT, lue, local_counts[rank], MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(&ve, local_counts, offsets,MPI_LONG_LONG_INT, lve, local_counts[rank], MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(&we, local_counts, offsets,MPI_LONG_LONG_INT, lwe, local_counts[rank], MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    for(ll i = 0; i< n-1;i++)
    {
        for(ll j=0;j<local_counts[rank];j++)
        {
            ll u = lue[j];
            ll v = lve[j];
            ll w = lwe[j];
            if(dist[u] != LLONG_MAX && (dist[u]+w)<dist[v] )
                dist[v] = dist[u] + w;
        }
        ll ldist[n+1];
        MPI_Allreduce(&dist,&ldist,n+1,MPI_LONG_LONG_INT,MPI_MIN,MPI_COMM_WORLD);
        for(int j=0;j<=n;j++)
            dist[j] = ldist[j];
    }

    if(rank == 0)
    {
        ofstream out;
        out.open(argv[2]);
        for(int i=1;i<=n;i++)
        {
            if(dist[i]==LLONG_MAX)
                out << i << " -1\n";
            else
                out << i << " " << dist[i] << "\n";
        }
        out.close();
    }


    MPI_Barrier( MPI_COMM_WORLD );
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        printf( "Total time (s): %f\n", maxTime );
    }

    /* shut down MPI */
    MPI_Finalize();
    return 0;
}