#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_NUMBERS 1000
#define MAX_WORKERS 10

// message structure for message queue
typedef struct
{
    long msg_type;
    int numbers[MAX_NUMBERS];
    int count;
} message_t;

// global variables
int total_sum = 0;
int num_workers;
int worker_pipes[MAX_WORKERS][2];
pid_t worker_pids[MAX_WORKERS];

// signal handler for SIGUSR1
void signal_handler(int signo)
{
    // do nothing, just wake up the process
}

// worker process function
void worker(int pipe_fd, int msgid, int worker_id)
{
    // install signal handler
    signal(SIGUSR1, signal_handler);

    // wait for SIGUSR1
    pause();

    // receive numbers from the message queue
    message_t msg;

    if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), worker_id + 1, 0) == -1)
    {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    // compute partial sum
    int partial_sum = 0;
    printf("the list of numbers for worker number %d is: [", worker_id);
    for (int i = 0; i < msg.count; i++)
    {
        printf("%d,", msg.numbers[i]);
        partial_sum += msg.numbers[i];
    }
    printf("] ,the partial sum is %d \n", partial_sum);

    // send the partial sum back to the manager
    if (write(pipe_fd, &partial_sum, sizeof(partial_sum)) == -1)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }
    exit(0);
}

int main()
{
    // input number of workers
        printf("Enter number of workers: ");
        scanf("%d", &num_workers);

        if (num_workers <= 1 || num_workers > MAX_WORKERS)
        {
            fprintf(stderr, "Invalid number of workers.\n");
            return EXIT_FAILURE;
        }

        // input numbers
        int numbers[MAX_NUMBERS], count = 0;
        printf("Enter numbers (end with -1): ");
        while (scanf("%d", &numbers[count]) && numbers[count] != -1)
        {
            count++;
        }

        if (count == 0)
        {
            fprintf(stderr, "No numbers entered.\n");
            return EXIT_FAILURE;
        }

        // create a message queue
        int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
        if (msgid == -1)
        {
            perror("msgget");
            return EXIT_FAILURE;
        }

        // create worker processes
        for (int i = 0; i < num_workers; i++)
        {
            if (pipe(worker_pipes[i]) == -1)
            {
                perror("pipe");
                return EXIT_FAILURE;
            }

            if ((worker_pids[i] = fork()) == 0)
            {
                // worker process
                close(worker_pipes[i][0]); // Close read end
                worker(worker_pipes[i][1], msgid, i);
            }
            // close write end in the manager
            close(worker_pipes[i][1]);
        }

        // distribute numbers to workers
        int start = 0; // start index for each worker
        for (int i = 0; i < num_workers; i++)
        {
            message_t msg;
            msg.msg_type = i + 1;

            // calculate chunk size
            int chunk_size = count / num_workers;
            if (i < count % num_workers)
            {
                // distribute remaining numbers
                chunk_size++;
            }
            msg.count = chunk_size;

            // copy numbers to the message
            for (int j = 0; j < chunk_size; j++)
            {
                msg.numbers[j] = numbers[start + j];
            }
            start += chunk_size;

            // send message to worker
            if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1)
            {
                perror("msgsnd");
                return EXIT_FAILURE;
            }

            // signal the worker to start
            kill(worker_pids[i], SIGUSR1);
            usleep(1000); // add slight delay to ensure proper signaling
        }

        // collect results from workers
        for (int i = 0; i < num_workers; i++)
        {
            int partial_sum = 0;
            if (read(worker_pipes[i][0], &partial_sum, sizeof(partial_sum)) == -1)
            {
                perror("read");
                return EXIT_FAILURE;
            }
            total_sum += partial_sum;
        }

        // wait for all workers to finish
        for (int i = 0; i < num_workers; i++)
        {
            wait(NULL);
        }

        // display the total sum
        printf("Total sum: %d\n", total_sum);
        printf("the list delievered from the manager :[");

        for (int l = 0; l < count; l++)
        {
            printf("%d,", numbers[l]);
        }
        printf("]");

        // cleanup
        if (msgctl(msgid, IPC_RMID, NULL) == -1)
        {
            perror("msgctl");
            return EXIT_FAILURE;
        }
    
    return 0;
}
