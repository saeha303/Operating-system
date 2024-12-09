#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <random>
using namespace std;
pthread_t* students;
FILE* outFile;
static random_device rd;
static mt19937 generator(rd());
int N, M;
int w, x, y;
int curr;
int *stud;    // printing state
int *binding; // binding state
int *binded;
int *print_wait;
int it = 0;
int submission = 0;
int ps[4]={0,0,0,0};
sem_t *ps_priority;
sem_t bs[2];
sem_t *finishedPrint;
sem_t *finished_binding;
sem_t db;
sem_t mutex;
int rc = 0;
int endProgram=0;
// vector<int> q[4];

void init_semaphore()
{
    // for (int i = 0; i < 4; i++)
    // {
    //     sem_init(&ps[i], 0, 1);
    // }
    for (int i = 0; i < N / M; i++)
    {
        sem_init(&finishedPrint[i], 0, 0);
        sem_init(&finished_binding[i], 0, 0);
        // sem_init(&ps_priority[i], 0, 1);
    }
    for (int i = 0; i < N; i++)
    {
        sem_init(&ps_priority[i], 0, 0);
    }
    for (int i = 0; i < 2; i++)
    {
        sem_init(&bs[i], 0, 1);
    }
    sem_init(&db, 0, 1);
    sem_init(&mutex, 0, 1);
    // pthread_mutex_init(&lock,0);
}
int my_random(double arg)
{
    double lambda_param = arg;

    // Create a random number generator with a Poisson distribution
    // default_random_engine generator;
    poisson_distribution<int> poissonDist(lambda_param);

    // Generate random numbers from the Poisson distribution
    return poissonDist(generator);
}
void print(int student)
{
    int pr_st = student % 4 + 1;
    bool flag = false;
    int grp = (int)ceil((double)student / M);
    if(ps[pr_st-1]==0){
    sem_post(&ps_priority[student - 1]);
    }
    sem_wait(&ps_priority[student - 1]);
    // another semaphore based on queue
    // sem_wait(&ps[pr_st - 1]);
    ps[pr_st-1]=1;
    print_wait[student - 1] = 0;
    int spent = time(0) - curr;
    // q[student % 4].push_back(student);
    fprintf(outFile,"Student %d has started printing at the print station %d at time %d\n", student, pr_st, spent);
    sleep(w);
    fprintf(outFile,"Student %d has finished printing at time %d\n", student, spent + w);
    ps[pr_st-1]=0;
    // sem_post(&ps[pr_st - 1]);
    stud[student - 1] = 1;
    
    for (int i = 0; i < N; i++)
    {
        double pid = ((double)(student));
        double oid = ((double)(i + 1));
        if (pid == oid)
            {
                sem_post(&ps_priority[i]);
                // flag = true;
            }
        if (student % 4 == (i + 1) % 4 && print_wait[i])
        {
            if (pid != oid && ceil(pid / M) == ceil(oid / M))
            {
                fprintf(outFile,"Student %d has notified %d of own team at the print station %d\n", student, i + 1, pr_st);
                sem_post(&ps_priority[i]);
                // flag = true;
                break;
            }
            else{
                fprintf(outFile,"Student %d has notified %d of other team at the print station %d\n", student, i + 1, pr_st);
                sem_post(&ps_priority[i]);
                break;
            }
        }
    }
    // sleep(my_random(5.0));
}
void *task1(void *arg)
{
    
    int rand = my_random(5.0);
    sleep(rand);
    
    int student = *((int *)arg);
    int pr_st = student % 4 + 1;
    print_wait[student - 1] = 1;
    int spent = time(0) - curr;
    // q[student % 4].push_back(student);
    fprintf(outFile,"Student %d has arrived at the print station %d at time %d\n", student, pr_st, spent);
    print(student);
    if(student%M==0){
        double pid=((double)(student));
        double gid=ceil(pid/M);
        for(int i=(gid-1)*M;i<gid*M-1;i++){
            pthread_join(students[i],NULL);
        }
    }
    int count = 0;
    for (int i = 0; i < N; i++)
    {
        double pid = ((double)(student));
        double oid = ((double)(i + 1));

        if (ceil(pid / M) == ceil(oid / M) && stud[i] == 1)
        {
            count++;
            if (count == M && !binding[(int)ceil(((double)student) / M) - 1]) //((int)pid % M == 0 || (int)oid % M == 0) &&
            {
                binding[(int)ceil(((double)student) / M) - 1] = 1;
                sem_post(&finishedPrint[(int)ceil(((double)student) / M) - 1]);
                break;
            }
        }
    }
    free(arg);
    
    // prioritize queue
    // else
    //     pthread_exit(NULL);
}
void *bind_st_1(void *arg)
{
    int grp = *((int *)arg);
    sem_wait(&bs[0]);
    if (!binded[grp - 1] && binding[grp - 1])
    {
        // sem_wait(&bs[0]);
        binded[grp - 1] = 1;
        int spent = time(0) - curr;
        fprintf(outFile,"Group %d has started binding at binding station 1 at time %d\n", grp, spent);
        sleep(x);
        fprintf(outFile,"Group %d has finished binding at time %d\n", grp, spent + x);
        sem_post(&finished_binding[grp - 1]);
        // sem_post(&bs[0]);
    }
    // free(arg);
    sem_post(&bs[0]);
    pthread_exit(NULL);
}
void *bind_st_2(void *arg)
{
    int grp = *((int *)arg);
    sem_wait(&bs[1]);
    if (!binded[grp - 1] && binding[grp - 1])
    {
        // sem_wait(&bs[1]);
        binded[grp - 1] = 1;
        int spent = time(0) - curr;
        fprintf(outFile,"Group %d has started binding at binding station 2 at time %d\n", grp, spent);
        sleep(x);
        fprintf(outFile,"Group %d has finished binding at time %d\n", grp, spent + x);
        sem_post(&finished_binding[grp - 1]);
        // sem_post(&bs[1]);
    }
    // free(arg);
    sem_post(&bs[1]);
    // }
    pthread_exit(NULL);
}
void *task2(void *arg)
{
    int grp = *((int *)arg);
    sem_wait(&finishedPrint[grp - 1]);
    //
    if (!binded[grp - 1] && binding[grp - 1])
    {
        pthread_t bs1;
        pthread_t bs2;
        pthread_create(&bs1, NULL, bind_st_1, (void *)arg);
        pthread_create(&bs2, NULL, bind_st_2, (void *)arg);
        pthread_join(bs1, NULL);
        pthread_join(bs2, NULL);
    }
    pthread_exit(NULL);
}
void *task3_write(void *arg)
{
    int grp = *((int *)arg);
    sem_wait(&finished_binding[grp - 1]);
    if (binded[grp - 1])
    {
        sem_wait(&db);
        int spent = time(0) - curr;

        sleep(y);
        submission++;
        sem_post(&db);
        fprintf(outFile,"Group %d has submitted the report at time %d\n", grp, spent + y);
    }
    pthread_exit(NULL);
}
void *staff_read(void *arg)
{
    int rand;
    int st = *((int *)arg);
    while (true)
    {

        rand = my_random(3.5);
        sleep(rand);
        sem_wait(&mutex);
        rc++;
        if (rc == 1)
            sem_wait(&db);
        sem_post(&mutex);
        int sub = submission;
        sleep(y);
        sem_wait(&mutex);
        rc--;
        if (!rc)
            sem_post(&db);
        sem_post(&mutex);
        fprintf(outFile,"Staff %d has started reading the entry book at time %ld. No. of submission = %d\n", st, time(0) - curr, sub);
        if (sub == N / M){
            break;
        }
    }
    pthread_exit(NULL);
}
void *task3_read(void *smth)
{
    pthread_t staff[2];
    int *arg;
    arg = (int *)malloc(sizeof(int));
    *arg = 1;
    pthread_create(&staff[0], NULL, staff_read, (void *)arg);
    sleep(my_random(5.0));
    arg = (int *)malloc(sizeof(int));
    *arg = 2;
    pthread_create(&staff[1], NULL, staff_read, (void *)arg);
    pthread_join(staff[0],NULL);
    pthread_join(staff[1],NULL);
    pthread_exit(NULL);
}
int main()
{
    // Create an input file stream object
    ifstream inputFile("in.txt");
    outFile = fopen("out.txt", "w");
    // Check if the file is opened successfully
    if (!inputFile.is_open())
    {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    // Read the first line containing two integers
    string line;
    getline(inputFile, line);
    istringstream iss(line);
    // int num1, num2;
    if (!(iss >> N >> M))
    {
        cerr << "Error reading integers from the first line." << endl;
        return 1;
    }

    // Read the second line containing three integers
    getline(inputFile, line);
    istringstream iss2(line);
    if (!(iss2 >> w >> x >> y))
    {
        cerr << "Error reading integers from the second line." << endl;
        return 1;
    }

    // Display the read values
    // cout<<N<<M<<w<<x<<y<<'\n';
    int *arg;
    stud = new int[N];
    for (int i = 0; i < N; i++)
    {
        stud[i] = 0;
    }
    students=new pthread_t[N];
    pthread_t group[N / M];
    // staff thread
    pthread_t read_thread;
    pthread_create(&read_thread, NULL, task3_read, (void *)arg);
    // pthread_join(read_thread, NULL);
    finishedPrint = new sem_t[N / M];
    finished_binding = new sem_t[N / M];
    ps_priority = new sem_t[N];
    binding = new int[N / M];
    binded = new int[N / M];
    print_wait = new int[N];
    int allThreadDone[N/M];
    for (int i = 0; i < N / M; i++)
    {
        binding[i] = 0;
        binded[i] = 0;
        allThreadDone[i]=0;
    }
    for (int i = 0; i < N; i++)
    {
        
        print_wait[i] = 0;
    }
    init_semaphore();
    // task 1
    curr = time(0);
    for (int i=0;i<N;i++) // int i : numbers
    {
        arg = (int *)malloc(sizeof(int));
        *arg = i + 1;
        // int rand = my_random(5.0);
        // sleep(rand);
        pthread_create(&students[i], NULL, task1, (void *)arg);
        // pthread_join(students[i],NULL);
        arg = (int *)malloc(sizeof(int));
        *arg = (int)ceil(((double)i + 1) / M);
        if(!allThreadDone[(int)ceil(((double)i + 1) / M) - 1])
            pthread_create(&group[(int)ceil(((double)i + 1) / M) - 1], NULL, task2, (void *)arg);
        // pthread_join(group[(int)ceil(((double)i + 1) / M) - 1], NULL);
        arg = (int *)malloc(sizeof(int));
        *arg = (int)ceil(((double)i + 1) / M);
        if(!allThreadDone[(int)ceil(((double)i + 1) / M) - 1])
            pthread_create(&group[(int)ceil(((double)i + 1) / M) - 1], NULL, task3_write, (void *)arg);
        // pthread_join(group[(int)ceil(((double)i + 1) / M) - 1], NULL);
        allThreadDone[(int)ceil(((double)i + 1) / M) - 1]=1;
    }
    for(int i=0;i<N;i++){
        if((i+1)%M==0)
            pthread_join(students[i],NULL);
        if(allThreadDone[(int)ceil(((double)i + 1) / M) - 1]){
            pthread_join(group[(int)ceil(((double)i + 1) / M) - 1], NULL);
            pthread_join(group[(int)ceil(((double)i + 1) / M) - 1], NULL);
            allThreadDone[(int)ceil(((double)i + 1) / M) - 1]=0;
        }
    }
    pthread_join(read_thread, NULL);
    // while (1)
    //     ;
    // Close the file
    inputFile.close();
    fclose(outFile);
    delete[] students;
    delete[] ps_priority;
    delete[] finished_binding;
    delete[] finishedPrint;
    delete[] stud;
    delete[] binding;
    delete[] binded;
    return 0;
}
