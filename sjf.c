#include <stdio.h>

int main()
{
    int n;

    printf("Enter number of processes: ");
    scanf("%d", &n);

    int bt[n];
    int ct[n];
    int tat[n];
    int wt[n];

    printf("\nEnter burst time for each process:\n");

    for (int i = 0; i < n; i++)
    {
        printf("P%d: ", i + 1);
        scanf("%d", &bt[i]);
    }

    int time = 0;

    printf("\nGantt Chart:\n");

    printf("0");

    for (int i = 0; i < n; i++)
    {
        printf(" | P%d |", i + 1);

        time += bt[i];

        ct[i] = time;
        tat[i] = ct[i];          // Arrival time = 0
        wt[i] = tat[i] - bt[i];

        printf(" %d", time);
    }

    printf("\n");

    float total_wt = 0;
    float total_tat = 0;

    printf("\nProcess\tBT\tCT\tTAT\tWT\n");

    for (int i = 0; i < n; i++)
    {
        printf("P%d\t%d\t%d\t%d\t%d\n",
               i + 1,
               bt[i],
               ct[i],
               tat[i],
               wt[i]);

        total_wt += wt[i];
        total_tat += tat[i];
    }

    printf("\nAverage Waiting Time = %.2f",
           total_wt / n);

    printf("\nAverage Turnaround Time = %.2f\n",
           total_tat / n);

    return 0;
}
