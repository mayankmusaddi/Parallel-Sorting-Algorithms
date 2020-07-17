#include <stdio.h>

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

void quickSort(int arr[], int left, int right)
{
    if (left < right)
    {
        if(right-left <  5)
            insertionSort(arr,left,right);
        else
        {
            int pivot = partition(arr, left, right);
            quickSort(arr, left, pivot - 1);
            quickSort(arr, pivot + 1, right);
        }
    }
}

int main()  
{
    int n,arr[100005];
    scanf("%d",&n);

    for(int i=0;i<n;i++)
        scanf("%d",&arr[i]);

    quickSort(arr, 0, n - 1);

    for(int i=0;i<n;i++)
        printf("%d ",arr[i]);
    printf("\n");
    return 0;  
}