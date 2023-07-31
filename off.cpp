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
int N, M;
int w, x, y;
int curr;
int *stud;    // printing state
int *binding; // binding state
int *binded;
int it = 0;
int submission = 0;
sem_t ps[4];
sem_t bs[2];
sem_t *finishedPrint;
sem_t *finished_binding;
sem_t db;
sem_t mutex;
int rc=0;
vector<int> q[4];
void init_semaphore()
{
    for (int i = 0; i < 4; i++)
    {
        sem_init(&ps[i], 0, 1);
    }
    for (int i = 0; i < N / M; i++)
    {
        sem_init(&finishedPrint[i], 0, 0);
        sem_init(&finished_binding[i], 0, 0);
    }
    for (int i = 0; i < 2; i++)
    {
        sem_init(&bs[i], 0, 1);
    }
    sem_init(&db, 0, 1);
    sem_init(&mutex,0,1);
    // pthread_mutex_init(&lock,0);
}
int my_random(double arg)
{
    // Set the lambda parameter for the Poisson distribution
    // random_device rd;
    // default_random_engine generator0(rd());
    // uniform_real_distribution<double> distribution(0.0, 4.0);

    // // Generate a random fractional number between 0 and 10
    // double lambda_param = distribution(generator0);
    srand(time(0));
    int arr[10];
    double lambda_param = arg;

    // Create a random number generator with a Poisson distribution
    default_random_engine generator1;
    poisson_distribution<int> poissonDist(lambda_param);

    // Generate random numbers from the Poisson distribution
    for (int i = 0; i < 10; i++)
    {
        arr[i] = poissonDist(generator1);
    }
    return arr[rand() % 10];
}
void print(int student)
{
    int pr_st = student % 4 + 1;
    // printf("%d\n", pr_st);
    // another semaphore based on queue
    sem_wait(&ps[pr_st - 1]);

    int spent = time(0) - curr;
    printf("Student %d has arrived at the print station %d at time %d\n", student, pr_st, spent);
    stud[student - 1] = 1;
    // sleep(1);
    sem_post(&ps[pr_st - 1]);
    // printf("ok\n");

    sleep(w);
    printf("Student %d has finished printing at time %d\n", student, spent + w);
    for (int i = 0; i < q[pr_st - 1].size(); i++)
    {
        double pid = ((double)(student));
        double oid = ((double)(q[pr_st - 1][i]));
        if (pid != oid && ceil(pid / M) == ceil(oid / M))
        {
            // printf("Student %d has notified %d of own team at the print station %d\n", student, q[pr_st - 1][i], pr_st);
            
        }
    }
    for (int i = 0; i < q[pr_st - 1].size(); i++)
    {
        double pid = ((double)(student));
        double oid = ((double)(q[pr_st - 1][i]));
        if (ceil(pid / M) != ceil(oid / M))
        {
            // printf("Student %d has notified %d of other team at the print station %d\n", student, q[pr_st - 1][i], pr_st);
        }
    }
}
// void bind()
// {
//     sem_wait(&finishedPrint);
// }
void *task1(void *arg)
{
    // printf("hi %d\n", *((int *)arg));
    int student = *((int *)arg);
    print(student);

    int count = 0;
    for (int i = 0; i < N; i++)
    {
        double pid = ((double)(student));
        double oid = ((double)(i + 1));

        if (ceil(pid / M) == ceil(oid / M) && stud[i] == 1)
        {
            count++;
            printf("%f %f\n", pid, oid);
            if (count == M && !binding[(int)ceil(((double)student) / M) - 1]) //((int)pid % M == 0 || (int)oid % M == 0) &&
            {
                binding[(int)ceil(((double)student) / M) - 1] = 1;
                printf("%d shesh\n", student);
                sem_post(&finishedPrint[(int)ceil(((double)student) / M) - 1]);
                break;
            }
        }
    }
    free(arg);
    // prioritize queue
    pthread_exit(NULL);
}
void *bind_st_1(void *arg)
{
    int grp = *((int *)arg);
    printf("%d\n", grp);
    // while (true)
    // {
    sem_wait(&bs[0]);
    if (!binded[grp - 1] && binding[grp - 1])
    {
        // sem_wait(&bs[0]);
        binded[grp - 1] = 1;
        int spent = time(0) - curr;
        printf("Group %d has started binding at binding station 1 at time %d\n", grp, spent);
        sleep(x);
        printf("Group %d has finished binding at time %d\n", grp, spent + x);
        sem_post(&finished_binding[grp - 1]);
        // sem_post(&bs[0]);
    }
    // free(arg);
    sem_post(&bs[0]);
    // }
    pthread_exit(NULL);
}
void *bind_st_2(void *arg)
{
    int grp = *((int *)arg);
    printf("%d\n", grp);
    // while (true)
    // {
    sem_wait(&bs[1]);
    if (!binded[grp - 1] && binding[grp - 1])
    {
        // sem_wait(&bs[1]);
        binded[grp - 1] = 1;
        int spent = time(0) - curr;
        printf("Group %d has started binding at binding station 2 at time %d\n", grp, spent);
        sleep(x);
        printf("Group %d has finished binding at time %d\n", grp, spent + x);
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
        // int spent = time(0) - curr;
        // printf("Group %d has started binding at time %d\n", grp, spent);
        // sleep(x);
        // printf("Group %d has finished binding at time %d\n", grp, spent + x);
        // free(arg);
        pthread_join(bs1, NULL);
        pthread_join(bs2, NULL);
        // binding done
        //  sem_post(&finished_binding[grp-1]);
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
        submission++;
        sem_post(&db);
    }
    pthread_exit(NULL);
}
void *staff_read(void *arg)
{
    int rand;
    int st=*((int *)arg);
    while(true){

        rand=my_random(3.5);
        printf("rand: %d\n",rand);
        sleep(rand);
        sem_wait(&mutex);
        rc++;
        if(rc==1)
        sem_wait(&db);
        sem_post(&mutex);
        int sub=submission;
        sem_wait(&mutex);
        rc--;
        if(!rc)
        sem_post(&db);
        sem_post(&mutex);
        printf("Staff %d has started reading the entry book at time %ld. No. of submission = %d\n",st,time(0)-curr,sub);
    }
}
void *task3_read(void *pthread_attr_getstack)
{
    pthread_t staff[2];
    // int rand;
    // while (true)
    // {
    //     rand = my_random();
    //     sleep(rand);
        int *arg;
        arg = (int *)malloc(sizeof(int));
        *arg = 1;
        pthread_create(&staff[0], NULL, staff_read, (void *)arg);
        // rand = my_random();
        sleep(my_random(6.0));
        arg = (int *)malloc(sizeof(int));
        *arg = 2;
        pthread_create(&staff[1], NULL, staff_read, (void *)arg);
        pthread_exit(NULL);
    // }
}
int main()
{
    // Create an input file stream object
    ifstream inputFile("in.txt");

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
    pthread_t students[N];
    pthread_t group[N / M];
    // pthread_t staff[2];
    pthread_t read_thread;
    pthread_create(&read_thread, NULL, task3_read, (void *)arg);
    finishedPrint = new sem_t[N / M];
    finished_binding = new sem_t[N / M];
    binding = new int[N / M];
    binded = new int[N / M];
    for (int i = 0; i < N / M; i++)
    {
        binding[i] = 0;
        binded[i] = 0;
    }
    init_semaphore();
    // task 1
    vector<int> numbers;
    for (int i = 0; i < N; ++i)
    {
        numbers.push_back(i);
    }

    // Use a random_device to seed the random number generator
    random_device rd;
    mt19937 generator(rd());

    // Shuffle the vector using the Fisher-Yates algorithm
    shuffle(numbers.begin(), numbers.end(), generator);
    curr = time(0);
    for (int i : numbers) // int i : numbers
    {
        arg = (int *)malloc(sizeof(int));
        *arg = i + 1;
        int rand = my_random(3.5);
        // printf("my_random: %d\n", rand);
        q[*arg % 4].push_back(*arg);
        sleep(rand);
        pthread_create(&students[i], NULL, task1, (void *)arg);
        // pthread_join(students[i], NULL);
        arg = (int *)malloc(sizeof(int));
        *arg = (int)ceil(((double)i + 1) / M);
        // int count = 0;
        // for (int j = 0; j != *arg-1 && j < N; j++)
        // {
        //     double pid = ((double)(*arg));
        //     double oid = ((double)(j + 1));
        //     if (ceil(pid / M) == ceil(oid / M))
        //     {
        //         count++;
        //         if (count == M)
        //         {
        //             sem_post(&finishedPrint);
        //             break;
        //         }
        //     }
        // }
        // printf("for %f of %d group\n",(double)i+1,(int)ceil(((double)i+1) / M));
        pthread_create(&group[(int)ceil(((double)i + 1) / M) - 1], NULL, task2, (void *)arg);
        arg = (int *)malloc(sizeof(int));
        *arg = (int)ceil(((double)i + 1) / M);
        pthread_create(&group[(int)ceil(((double)i + 1) / M) - 1], NULL, task3_write, (void *)arg);
        // pthread_create(&staff[0], NULL, task2, (void *)arg);
        // pthread_create(&staff[1], NULL, task2, (void *)arg);

        // pthread_join(group[(int)ceil(((double)i+1) / M) - 1], NULL);
    }
    while (1)
        ;
    // Close the file
    inputFile.close();

    return 0;
}
