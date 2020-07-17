#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

struct arg{
    int left;
    int right;
    int* arr;    
};

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
    for (int j=left;j<right;j++)
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

void *quickSort(void * a)
{
    struct arg *args = (struct arg*) a;
    int left = args->left;
    int right = args->right;
    int *arr = args->arr;

    if (left < right)
    {
        if(right-left <  5)
            insertionSort(arr,left,right);
        else
        {
            int pivot = partition(arr, left, right);
            //sort left half array
            struct arg a1;
            a1.left = left;
            a1.right = pivot-1;
            a1.arr = arr;
            pthread_t tid1;
            pthread_create(&tid1, NULL, quickSort, &a1);
            
            //sort right half array
            struct arg a2;
            a2.left = pivot+1;
            a2.right = right;
            a2.arr = arr;
            pthread_t tid2;
            pthread_create(&tid2, NULL, quickSort, &a2);
            
            //wait for the two halves to get sorted
            pthread_join(tid1, NULL);
            pthread_join(tid2, NULL);
        }
    }
}

int main()  
{
    int n,arr[100005];
    scanf("%d",&n);

    for(int i=0;i<n;i++)
        scanf("%d",&arr[i]);

    pthread_t tid;
    struct arg a;
    a.left = 0;
    a.right = n-1;
    a.arr = arr;
    pthread_create(&tid, NULL, quickSort, &a);
    pthread_join(tid, NULL);

    for(int i=0;i<n;i++)
        printf("%d ",arr[i]);
    printf("\n");
    return 0;  
}