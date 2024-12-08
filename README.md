# Manager-Worker Sum Calculation Program

This program demonstrates inter-process communication (IPC) using message queues, pipes, and signals in C. The manager process divides a list of numbers into smaller portions and assigns them to worker processes. Each worker computes the sum of its assigned portion, and the manager collects the results to compute the final total sum.

## Features
- Manager divides the list into smaller portions for workers.
- Workers compute partial sums and send the results back to the manager.
- Communication between processes is done using:
  - Message queues for data distribution.
  - Pipes for sending results.
  - Signals (`SIGUSR1` and `SIGUSR2`) for synchronization.
- The manager computes and displays the final total sum.

## Requirements
- C compiler (GCC or similar)
- Linux/Unix-based OS for process management and IPC features

## How to Run

1. Clone the repository:
    ```bash
    git clone https://github.com/your-username/manager-worker-sum.git
    cd manager-worker-sum
    ```

2. Compile the program:
    ```bash
    gcc -o sum_program sum_program.c
    ```

3. Run the program:
    ```bash
    ./sum_program
    ```

## Example Output

Enter number of workers: 3 Enter numbers (end with -1): 12 12 3 5 2 -1 The list of numbers for worker number 0 is: [12, 12], the partial sum is 24 The list of numbers for worker number 1 is: [3, 5], the partial sum is 8 The list of numbers for worker number 2 is: [2], the partial sum is 2 Total sum: 34
