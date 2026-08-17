#include <stdio.h>

int main() {
    int n, q, i, time = 0, done = 0, front = 0, rear = 0;
    int at[20], bt[20], rt[20], ct[20], tat[20], wt[20], queue[100];
    int added[20] = {0};

    printf("Enter number of processes: ");
    scanf("%d", &n);

    for(i = 0; i < n; i++) {
        printf("P%d Arrival Time: ", i + 1);
        scanf("%d", &at[i]);
        printf("P%d Burst Time: ", i + 1);
        scanf("%d", &bt[i]);
        rt[i] = bt[i];
    }

    printf("Enter Time Quantum: ");
    scanf("%d", &q);

    while(done < n) {
        for(i = 0; i < n; i++)
            if(!added[i] && at[i] <= time) {
                queue[rear++] = i;
                added[i] = 1;
            }

        if(front == rear) {
            time++;
            continue;
        }

        i = queue[front++];

        if(rt[i] > q) {
            time += q;
            rt[i] -= q;
        } else {
            time += rt[i];
            rt[i] = 0;
            ct[i] = time;
            done++;
        }

        for(int j = 0; j < n; j++)
            if(!added[j] && at[j] <= time) {
                queue[rear++] = j;
                added[j] = 1;
            }

        if(rt[i] > 0)
            queue[rear++] = i;
    }

    printf("\nP\tAT\tBT\tCT\tTAT\tWT\n");

    for(i = 0; i < n; i++) {
        tat[i] = ct[i] - at[i];
        wt[i] = tat[i] - bt[i];

        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               i + 1, at[i], bt[i], ct[i], tat[i], wt[i]);
    }

    return 0;
}
