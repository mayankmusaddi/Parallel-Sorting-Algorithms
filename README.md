# Parallel Quick Sort (C++)

## Description
A parallel approach has been used in the standard Quick Sort Algorithm as per the algorithm described in "http://www.winlab.rutgers.edu/~pkataria/pdc.pdf". The essence of parallelization lies in the fact that the complete array is divided into equal chunks and they are sorted in different processes in parallel. The sorted chunks are sent back to process 0 which merges those chunks using a min heap as follows :-
- Push the minimum element of each sorted chunk into a min heap.
- Take out the minimum element from the heap and add it into sorted list. 
- Next element from the chunk to which this element belonged to, is added into the heap.
- Above 2 steps is repeated until the priority queue is empty.

## Analysis
For an array of 10000 elements and 4 processes, time taken data is :
- Sequential quick sort : 0.138 seconds
- Parallel quick sort : 0.092 seconds

# Parallel Single-Source Shortest Path (C++)

## Description
A parallel approach to Bellman Ford algorithm is implemented for an undirected graph. All the edges are distributed into equal chunks among processes. Each process relaxes their set of edges and updates the distance vector as per the standard bellman ford algorithm. Now these distance vectors from all the processes are combined by taking the minimum of all vectors across indices and then redistributed. Initially, all values in wt array is infinity except source vertex for which it is zero. Now, following process is repeated n times where n is number of vertices of the graph :
- Parent process broadcasts the dist array to all other processes.
- Each processes then iterate to all their respective edges and updates their dist array. Suppose they have an edge u to v with weight w, then dist[u] is made min(dist[u], w+dist[v]). (Other way around i.e. v to u is done in adjacency list of v).
- After updation, each process sends back their local dist arrays to process 0.
- Process 0 thus takes p-1 arrays (p being number of processes), takes minimum for each index of the array and updates its dist array. (done by MPI_AllReduce)
After this procedure, the final dist array contains required minimum distance.

## Analysis
For 1000 vertices and 10000 edges (random), time taken data is :
- Sequential Bellman-Ford : 0.1257 seconds
- Parallel Bellman-Ford : 0.091 seconds

# Parallel Merge Sort (Erlang)

Let total numbers are $N$ and each number is represented as $a_i$ ($0 \leq i \leq N-1$).
- Using recursive function `get_len`, value of $N$ is calculated.
- A new process is created which executed function `parallel_merge_sort` with arguments $N$, all numbers, depth ($=0$) and process id of parent process.
- `parallel_merge_sort` sorts the array given in arguments and sends it to process id of the parent.
- To sort, first it uses `partition` function to break given array in two almost equal parts. It then creates 2 new processes who recursively sorts these two partitions by executing the same `parallel_merge_sort` function.
- It then takes the two sorted array and combines them into one sorted array by using `merge` function.
- Finally, it sends the sorted array to the parent (using process id from the arguments).
- For every recursion, two new processes are created. To prevent the exponential growth of number of processes, variable `depth` is introduced.
- Depth denotes the recursion depth which in turn tells us number of processes created so far, which is $2^{depth+1}-1$. We fixed the upper limit of number of processes to $16$ and thus limit to depth is 3.
- Therefore, if $depth \geq 3$, then we do serial merge sort using `merge_sort` function.
- `merge_sort` sorts the array serially, by similar procedure. It recursively calls itself to sort two partitions and then uses `merge` function to combine them.
- Finally the main process (who called the function `parallel_merge_sort` from `main` function with complete list of numbers) gets the sorted numbers.

## Analysis

Parallel merge sort and serial merge sort were executed using a sample of $10^6$ numbers.
#### Time taken by Serial Merge Sort
```
real    0m16.621s
user    0m24.189s
sys     0m6.214s
```
#### Time taken by Parallel Merge Sort (with 15 processes)
```
real    0m21.015s
user    0m22.614s
sys     0m6.344s
```

# Concurrent QuickSort (C)

Quick Sort is a standard algorithm used for sorting used most commonly as it takes the least average time among other sorting algorithms. Here we have implemented 3 ways of quicksort and evaluate their performance analysis:

  - Standard Quicksort
  - Concurrent Quicksort using Processes
  - Concurrent Quicksort using Threading

#### 1) Standard Quicksort Algorithm
The standard algorithm follows the below given steps:
  - Pick the center element of the array as **pivot**
  - Now position the pivot in the array such the before it lies all the elements less than it and after it lies those greater than it
  - Now partition the array around the new position of the pivot
  - Run the algorithm again for the left and the right arrays using **Recursion**
  - If at any point the size of array is less than or equal to 5 then sort it using insertion sort

Running Instruction:

```sh
$ gcc quicksort.c
$ ./a.out
```

#### 2) Concurrent Quicksort using Processes
The algorithm is mostly same as the above except that separate processes are made for each recursion.
  - It uses **shmget**  (for shared memory allocation) and **shmat** (for shared memory operations) functions
  - The shmget()  requests the kernel to allocate a shared page for both the processes. shmat() attaches the System V shared memory segment identified by *shmid* to the address space of the calling process. 
  - We create a shared memory space between the child process that we fork.
  - Each segment is split into left and right child which is sorted and they are ran in different processes concurrently.
  - The traditional fork does not work because the child process and the parent process run in separate memory spaces and memory writes performed by one of the processes do not affect the other. Hence we need a shared memory segment.

Running Instruction:

```sh
$ gcc conc_quicksort.c
$ ./a.out
```

#### 2) Concurrent Quicksort using Threading
The algorithm is mostly same as the above except that separate threads are made for each recursion.
  - Threads are created using Pthreads
  - Separate threads are made for each Left and Right Segments that are made after the pivot.
  - Arrays and left and right index details are passed using Structs

Running Instruction:

```sh
$ gcc  thread_quicksort.c -lpthread
$ ./a.out
```

### Performance Analysis
The time taken for a randomly generated test case of array of size 10^4 is taken and the time is evaluated for each algorithm which are as follows:

| QuickSort Algorithm | real     | user     | sys      |
| ------              | ------   |  ------  |  ------  | 
| Standard            | 0m0.061s | 0m0.006s | 0m0.007s |
| Using Processes     | 0m0.435s | 0m2.409s | 0m0.474s |
| Using Threads       | 0m0.143s | 0m0.074s | 0m0.361s |

The time taken in *QuickSort using processes* is more than Standard Quicksort because of the high overhead cost of Processes Creation. This overhead is accounts for the cache miss i.e when, say left child, access the left array, the array is loaded into the cache of a processor. Now when the right array is accessed (because of concurrent accesses), there is a cache miss since the cache is filled  with left segment and then right segment is copied to the cache memory. This to-and-fro process continues and it degrades the performance to such a level that it performs poorer than the sequential code.

The time taken in *Quicksort using Threads* is more than the Standard Quicksort but less than that of Quicksort using processes due to lesser overhead cost of creating threads than processes. However it takes more time than Standard quicksort due to the fact that there are large amount of threads each of which is doing very little work, therefore  the overhead from creating all those threads far outweighs the gains we get from parallel processing. To speed this up we can make sure that each thread has a reasonable amount of work to do. For example, if one thread finds that it only has to sort 10000 numbers it can simply sort them itself with a normal QuickSort, instead of spawning new threads. As we increase the number of threads beyond the number of cores on the system, you get less and less benefit from the multiple threads.