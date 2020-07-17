#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void insertionSort(int arr[], int left,int right)
{
    for (int i=left+1;i<=right;i++)
    {
        int j,key = arr[i];
        for(j=i-1; j>=left && arr[j]>key ;j--)
            arr[j+1] = arr[j];
        arr[j+1] = key;
    }
}

int partition (int arr[], int left, int right)
{
    int pivot = arr[(left+right)/2];
    swap(&arr[right],&arr[(left+right)/2]);
    int i=(left-1);
    for (int j=left;j<=right;j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i],&arr[j]);
        }
    }
    swap(&arr[i+1],&arr[right]);
    return (i+1);
}

void quickSort(int arr[], int left, int right)
{
    // printf("Quick %d %d\n",left,right);
    if (left < right)
    {
        if(right-left <  5)
        {
        	// printf("insertionSort on %d %d \n",left,right);
            insertionSort(arr,left,right);
        }
        else
        {
            int pivot = partition(arr, left, right),status;
            // printf("Pivot : %d\n",pivot);
            int pid = fork(),pid2;
            if(pid==0)
            {
                quickSort(arr, left, pivot - 1);
                exit(0);
            }
            else
            {
                pid2 = fork();
                if(pid2==0)
                {
                	quickSort(arr, pivot + 1, right);
                    exit(0);
                }
            }
            waitpid(pid,&status,0);
            waitpid(pid2,&status,0);
        }
    }
}

int main()  
{
    int n;
    scanf("%d",&n);
    int shmid = shmget(IPC_PRIVATE,n*sizeof(int),IPC_CREAT|0666);
    int *arr = (int *) shmat(shmid,(void*)0,0); 

    for(int i=0;i<n;i++)
        scanf("%d",&arr[i]);

    quickSort(arr, 0, n - 1);

    for(int i=0;i<n;i++)
        printf("%d ",arr[i]);
    printf("\n");
    return 0;  
}