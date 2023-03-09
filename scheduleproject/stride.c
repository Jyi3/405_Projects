#include <stdio.h>
#include <stdlib.h>

// Define the structure for a task
typedef struct {
    int priority;  // Priority of the task
    int stride;    // Stride of the task
    int pass;      // Pass value of the task
    int executed;  // Number of times the task has executed
} task_t;

// Define the structure for a stride scheduler
typedef struct {
    task_t **tasks;  // Array of pointers to tasks
    int num_tasks;   // Number of tasks in the scheduler
    int quantum;     // Time quantum for each task
} stride_scheduler_t;

// Initialize a task
void task_init(task_t *task, int priority, int stride, int pass) {
    task->priority = priority;
    task->stride = stride;
    task->pass = pass;
    task->executed = 0;
}

// Create a stride scheduler
stride_scheduler_t *stride_scheduler_create(int num_tasks, int quantum) {
    stride_scheduler_t *scheduler = (stride_scheduler_t *) malloc(sizeof(stride_scheduler_t));
    scheduler->tasks = (task_t **) malloc(num_tasks * sizeof(task_t *));
    scheduler->num_tasks = num_tasks;
    scheduler->quantum = quantum;
    return scheduler;
}

// Add a task to the stride scheduler
void stride_scheduler_add_task(stride_scheduler_t *scheduler, int priority, int stride, int pass) {
    task_t *task = (task_t *) malloc(sizeof(task_t));
    task_init(task, priority, stride, pass);
    scheduler->tasks[priority] = task;
}

// Run the stride scheduler
void stride_scheduler_run(stride_scheduler_t *scheduler) {
    int current_pass = 0;
    int i;
    while (1) {
        task_t *current_task = NULL;
        int min_pass = 0x7fffffff;
        // Find the task with the smallest pass value
        for (i = 0; i < scheduler->num_tasks; i++) {
            if (scheduler->tasks[i] != NULL) {
                if (scheduler->tasks[i]->pass < min_pass) {
                    current_task = scheduler->tasks[i];
                    min_pass = current_task->pass;
                }
            }
        }
        // If there are no tasks left, exit the loop
        if (current_task == NULL) {
            break;
        }
        // Execute the current task for the quantum
        int j;
        for (j = 0; j < scheduler->quantum; j++) {
            current_task->executed++;
            if (current_task->executed % current_task->stride == 0) {
                current_task->pass += current_task->stride;
            }
        }
    }
}

// Print the execution counts for each task
void stride_scheduler_print_results(stride_scheduler_t *scheduler) {
    int i;
    for (i = 0; i < scheduler->num_tasks; i++) {
        if (scheduler->tasks[i] != NULL) {
            printf("Task %d executed %d times\n", i, scheduler->tasks[i]->executed);
        }
    }
}

// Free the memory used by the stride scheduler
void stride_scheduler_destroy(stride_scheduler_t *scheduler) {
    int i;
    for (i = 0; i < scheduler->num_tasks; i++) {
        if (scheduler->tasks[i] != NULL) {
            free(scheduler->tasks[i]);
        }
    }
    free(scheduler->tasks);
    free(scheduler);
}
