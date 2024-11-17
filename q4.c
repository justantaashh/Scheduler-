#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int startTime;
    int finishTime;
    int remainingTime;
    int hasStarted;
} Process;

int compare_arrivalTime(const void *a, const void *b)
{
    const Process *p1 = a;
    const Process *p2 = b;
    return (p1->arrivalTime > p2->arrivalTime) - (p1->arrivalTime < p2->arrivalTime);
}
int compare_burstTime(const void *a, const void *b)
{
    const Process *p1 = a;
    const Process *p2 = b;
    return (p1->burstTime > p2->burstTime) - (p1->burstTime < p2->burstTime);
}

void merge(Process arr[], int left, int mid, int right, int (*compare)(const void*, const void*)) 
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Process* L = (Process*)malloc(n1 * sizeof(Process));
    Process* R = (Process*)malloc(n2 * sizeof(Process));

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) 
    {
        if (compare(&L[i], &R[j]) <= 0)
            arr[k] = L[i++];
        else
            arr[k] = R[j++];
        k++;
    }
    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];
    free(L);
    free(R);
}

void mergeSort(Process arr[], int left, int right, int (*compare)(const void*, const void*))
{
    if (left < right) 
    {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid, compare);
        mergeSort(arr, mid + 1, right, compare);

        merge(arr, left, mid, right, compare);
    }
}

void print_details(Process processes[], int n) 
{
    double totalTurnaroundTime = 0, totalResponseTime = 0;
    printf("PID\tArrival\tBurst\tStart\tFinish\tResponse\tTurnaround\n");
    for (int i = 0; i < n; i++) 
    {
        int turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
        int responseTime = processes[i].startTime - processes[i].arrivalTime;
        printf("%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n",
               processes[i].pid, processes[i].arrivalTime, processes[i].burstTime,
               processes[i].startTime, processes[i].finishTime, responseTime, turnaroundTime);
        totalTurnaroundTime += turnaroundTime;
        totalResponseTime += responseTime;
    }
    printf("Average Response Time: %.2f\n", totalResponseTime / n);
    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime / n);
}

void initializeProcesses(Process processes[], int n) 
{
    for (int i = 0; i < n; i++) 
    {
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].startTime = -1;
        processes[i].finishTime = 0;
        processes[i].hasStarted = 0;
    }
}

void fifo(Process processes[], int n) 
{
    printf("FIFO Scheduling:\n");

    mergeSort(processes, 0, n - 1, (int (*)(const void*, const void*))compare_arrivalTime);
    
    int currentTime = 0;
    for (int i = 0; i < n; i++) 
    {
        if (currentTime < processes[i].arrivalTime)
            currentTime = processes[i].arrivalTime;

        processes[i].startTime = currentTime;

        processes[i].finishTime = currentTime + processes[i].burstTime;
        currentTime = processes[i].finishTime;
    }
    print_details(processes, n);
}

void sjf(Process processes[], int n) 
{
    printf("SJF (Non-preemptive) Scheduling:\n");

    mergeSort(processes, 0, n - 1, (int (*)(const void*, const void*))compare_arrivalTime);

    int currentTime = 0;
    int completed = 0;
    int processIndex = -1;
    int shortestBurst;

    while (completed < n) 
    {
        shortestBurst = __INT_MAX__;
        processIndex = -1;

        for (int i = 0; i < n; i++) 
        {
            if (processes[i].arrivalTime <= currentTime && !processes[i].finishTime && processes[i].burstTime < shortestBurst) 
            {
                shortestBurst = processes[i].burstTime;
                processIndex = i;
            }
        }

        if (processIndex == -1)
            currentTime++; 
        else 
        {
            if (processes[processIndex].startTime == -1)
                processes[processIndex].startTime = currentTime;

            processes[processIndex].finishTime = currentTime + processes[processIndex].burstTime;
            currentTime += processes[processIndex].burstTime;
            completed++;
        }
    }

    print_details(processes, n);
}

void srtf(Process processes[], int n) 
{
    printf("SRTF Scheduling:\n");
    int currentTime = 0, completed = 0;
    while (completed < n) 
    {
        int minIndex = -1;
        int minRemaining = __INT_MAX__;
        for (int i = 0; i < n; i++) 
        {
            if (processes[i].arrivalTime <= currentTime && processes[i].finishTime == 0 && processes[i].remainingTime < minRemaining) {
                minRemaining = processes[i].remainingTime;
                minIndex = i;
            }
        }
        if (minIndex == -1) 
        {
            currentTime++;
            continue;
        }
        processes[minIndex].remainingTime--;
        processes[minIndex].startTime = (processes[minIndex].hasStarted ? processes[minIndex].startTime : currentTime);
        if (processes[minIndex].remainingTime == 0) 
        {
            processes[minIndex].finishTime = currentTime + 1;
            completed++;
        }
        currentTime++;
        processes[minIndex].hasStarted = 1;
    }
    print_details(processes, n);
}

void roundRobin(Process processes[], int n, int tq) {
    printf("Round Robin Scheduling:\n");
    int currentTime = 0, completed = 0;
    while (completed < n) 
    {
        for (int i = 0; i < n; i++) 
        {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0) 
            {
                processes[i].startTime = (processes[i].hasStarted ? processes[i].startTime : currentTime);
                int runtime = (processes[i].remainingTime > tq) ? tq : processes[i].remainingTime;
                processes[i].remainingTime -= runtime;
                currentTime += runtime;
                if (processes[i].remainingTime == 0) 
                {
                    processes[i].finishTime = currentTime;
                    completed++;
                }
                processes[i].hasStarted = 1;
            }
        }
    }
    print_details(processes, n);
}

int main() 
{
    int n, tq;
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    Process processes[n];

    for (int i = 0; i < n; i++) 
    {
        printf("Enter Process ID, Arrival Time, Burst Time: ");
        scanf("%d %d %d", &processes[i].pid, &processes[i].arrivalTime, &processes[i].burstTime);
    }

    printf("Enter Time Quantum for Round Robin: ");
    scanf("%d", &tq);

    initializeProcesses(processes, n);
    fifo(processes, n);
    initializeProcesses(processes, n);
    sjf(processes, n);
    initializeProcesses(processes, n);
    srtf(processes, n);
    initializeProcesses(processes, n);
    roundRobin(processes, n, tq);

    return 0;
}
