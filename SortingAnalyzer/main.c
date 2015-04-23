#include <stdio.h>
#include <stdlib.h>
#include <time.h>
extern void swap(double *a, double *b);
extern int isl(double a, double b);
extern unsigned long long timestamp(void);

int movtact = 0;
int swaptact = 0;

/*
void swap(double *a, double *b)
{
    double c;
    c = *a;
    *a = *b;
    *b = c;
}
*/


int selection_sort(double data[], int count)
{
    int i, j;
    double tmp;
    int minimum;
    for (i = 0; i < count - 1; i++)
    {
        minimum = i; /* current minimum */

        /* find the global minimum */
        for (j = i + 1; j < count; j++)
            if (isl(data[j], data[minimum]))
                minimum = j; /* new minimum */
        /* swap data[i] and data[minimum] */
        swap(&data[i], &data[minimum]);
    }
    return 0;
}

void shell_sort (double *a, int n) {

    int h, i, j;
    double k;
// gap = h, it becomes twice smaller every time, and it is n/2 in the beginning
    for (h = n; h /= 2;) {
        //  let's start from half and to the end
        for (i = h; i < n; i++) {
            //  let's compare middle element with elements before that stand on gap
          //  k = a[i];
          //  for (j = i; j >= h && isl(k, a[j - h]); j -= h)
          //     a[j] = a[j - h];
          //  a[j] = k;
         //   k = a[i];
            for (j = i; j >= h && isl(a[j], a[j - h]); j -= h) {
               //k = a[j];
               //a[j] = a[j - h];
               //a[j - h] = k;
               swap(&a[j], &a[j - h]);
            }
        //    a[j] = k;
        }
    }
    return;
}


double* generate(int n, int param)
{
    int i;
    double* a = (double*) malloc(n * sizeof(double));

    srand(time(NULL));

    for(i = 0; i < n; i++) {
         a[i] = rand() * rand() * rand() * rand() * rand() + 0.0;
         a[i] /= rand() * rand() * rand() * rand() * rand() + 0.0;
    }

    if(param == 3 || param == 4)
        ;
    else if(param == 1)
        selection_sort(a, n);
    else {
        double* b = (double*) malloc(n * sizeof(double));
        selection_sort(a, n);
        for(i = 0; i < n; i++)
            b[i] = a[n - i];
        free(a);
        return b;
    }
    return a;
}

int main(void)
{
    int i, j, k, avermov1 = 0, averswap1 = 0, avermov2 = 0, averswap2 = 0;
    int n[] = {10, 100, 1000, 10000};
    unsigned long long avertime1 = 0, avertime2 = 0;
    double *arr, *arr2;
    unsigned long long t;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            arr = generate(n[i], j+1);

            arr2 = (double*) malloc(n[i]*sizeof(double));
            for(k = 0; k < n[i]; k++)
                arr2[k] = arr[k];
            movtact = 0;
            swaptact = 0;
            t = timestamp();
            selection_sort(arr, n[i]);
            t = timestamp() - t;

            printf("Selection sort\n");
            printf("time for array #%d %llu\n", j+1, t);
            printf("amount of calling swap function: %d\n", swaptact);
            printf("amount of comparing: %d\n", movtact);

            avermov1 += movtact;
            averswap1 += swaptact;
            avertime1 += t;
            movtact = 0;
            swaptact = 0;

            t = timestamp();
            shell_sort(arr2, n[i]);
            t = timestamp() - t;
            printf("\n");
            printf("Shell sort\n");
            printf("time for array #%d %llu\n", j+1, t);
            printf("amount of calling swap function: %d\n", swaptact);
            printf("amount of comparing: %d\n\n\n", movtact);

            avermov2 += movtact;
            averswap2 += swaptact;
            avertime2 += t;

            free(arr);
            free(arr2);
        }

        avermov1 /=4;
        avermov2 /=4;
        averswap1 /=4;
        averswap2 /=4;
        avertime1 /=4;
        avertime2 /=4;
        printf("\n\n");
        printf("Average values for Selection sort\n");
        printf("time %llu\n", avertime1);
        printf("calling swap function: %d\n", averswap1);
        printf("comparing: %d\n\n", avermov1);

        printf("\n");
        printf("Average values for Shell sort\n");
        printf("time %llu\n", avertime2);
        printf("calling swap function: %d\n", averswap2);
        printf("comparing: %d\n\n", avermov2);
        avermov1 =0;
        avermov2 =0;
        averswap1 =0;
        averswap2 =0;
        avertime1 =0;
        avertime2 =0;
    }
    //for(i = 0; i < 10; i++)
    //     printf("%f ", arr[i]);
    printf("\n");
    return 0;
}
