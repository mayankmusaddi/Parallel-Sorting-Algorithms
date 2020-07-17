/* MPI Program Template */

#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;

ll partition(ll *arr, ll l, ll r)
{
    ll pos = rand()%(r-l)+l;
    ll pivot = arr[pos];

    swap(arr[r],arr[pos]);
    ll i = l-1;
    for(ll j=l; j<=r; j++)
    {
        if(arr[j]<pivot)
        {
            i++;
            swap(arr[i],arr[j]);
        }
    }
    swap(arr[i+1],arr[r]);
    return i+1;
}
void quicksort(ll *arr, ll l, ll r)
{
    if(l<r)
    {
        ll pivot = partition( arr, l,r);
        quicksort( arr, l, pivot-1);
        quicksort( arr, pivot+1, r);
    }
}

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
    ll arr[100000];
    int local_counts[numprocs], offsets[numprocs];
    if(rank == 0)
    {
        ifstream inp;
        inp.open(argv[1]);

        int i=0;
        ll k;
        while(inp >> k)
        {
            arr[i] = k;
            i++;
        }
        inp.close();
        ll N=i;        

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

    ll localArray[local_counts[rank]];
    MPI_Scatterv(&arr, local_counts, offsets,MPI_LONG_LONG_INT, localArray, local_counts[rank], MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    quicksort(localArray,0,local_counts[rank]-1);

    ll sortedArr[100000];
    MPI_Gatherv( localArray, local_counts[rank], MPI_LONG_LONG_INT, sortedArr, local_counts, offsets, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    if(rank == 0)
    {
        priority_queue <pair <ll,ll>, vector<pair <ll,ll> > , greater <pair <ll,ll> > > q;
        for(int i=0;i<numprocs;i++)
            q.push(make_pair(sortedArr[offsets[i]], i));

        ofstream out;
        out.open(argv[2]);
        while(!q.empty())
        {
            pair<ll,ll> top = q.top();
            out << top.first << " ";
            q.pop();

            local_counts[top.second]--;
            if(local_counts[top.second]>0)
            {
                offsets[top.second]++;
                q.push(make_pair(sortedArr[offsets[top.second]],top.second));
            }
        }
        out << endl;
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