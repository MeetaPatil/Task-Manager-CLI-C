/*
 * Task Manager (CLI) in C
 *
 * Features:
 *  - Add tasks with priority (1=Low, 2=Medium, 3=High)
 *  - View all tasks
 *  - View tasks by priority (using a max-heap priority queue)
 *  - Show top-priority pending task
 *  - Mark task as complete
 *  - Undo last completion (stack / LIFO)
 *  - Soft delete tasks
 *
 * Data Structures Used:
 *  - Array of Task structs (acts like a simple database)
 *  - Max-Heap (priority queue) for high-priority first
 *  - Stack to support Undo
 *
 * Build:
 *      gcc task_manager.c -o task_manager
 *
 * Run:
 *      ./task_manager
 *
 * Notes:
 *  - This program uses stdin/stdout (terminal menu).
 *  - Max tasks = 100 (tweak MAX_TASKS).
 *  - All data is in-memory only (no file persistence yet).
 *
 * License:
 *  - You can use/modify this code in college projects / personal projects.
 *    If you publish, credit is appreciated but not required :)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100

/* ---------------------- Task Definition ---------------------- */
typedef struct {
    int id;             // unique ID for the task
    char title[50];     // short description/title
    int priority;       // 1=Low, 2=Medium, 3=High
    int done;           // 0 = pending, 1 = completed
    int active;         // 1 = exists, 0 = "deleted"
} Task;

/* ---------------------- Globals ---------------------- */
Task tasks[MAX_TASKS];
int taskCount = 0;
int nextId = 1;

/* Stack for Undo (stores last completed task IDs) */
int undoStack[MAX_TASKS];
int top = -1;

/* Max-Heap for Priority Queue (stores indexes of tasks[]) */
int heap[MAX_TASKS];
int heapSize = 0;

/* ---------------------- Stack Operations ---------------------- */
void pushUndo(int id) {
    if (top < MAX_TASKS - 1) {
        undoStack[++top] = id;
    }
}

int popUndo() {
    if (top >= 0) {
        return undoStack[top--];
    }
    return -1; // empty
}

/* ---------------------- Heap Helpers ---------------------- */

/*
 * swap: swap two integers
 */
void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/*
 * comparePriority:
 * returns 1 if tasks[i] has STRICTLY higher priority than tasks[j]
 * (used by the max-heap)
 */
int comparePriority(int i, int j) {
    return tasks[i].priority > tasks[j].priority;
}

/*
 * heapifyUp:
 * bubble a node up to maintain max-heap property
 */
void heapifyUp(int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (comparePriority(heap[idx], heap[parent])) {
            swap(&heap[idx], &heap[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

/*
 * heapifyDown:
 * push a node down to maintain max-heap property
 */
void heapifyDown(int idx) {
    while (1) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int largest = idx;

        if (left < heapSize && comparePriority(heap[left], heap[largest])) {
            largest = left;
        }
        if (right < heapSize && comparePriority(heap[right], heap[largest])) {
            largest = right;
        }

        if (largest != idx) {
            swap(&heap[idx], &heap[largest]);
            idx = largest;
        } else {
            break;
        }
    }
}

/*
 * insertHeap:
 * pushes the index of a task (in tasks[]) into the heap
 */
void insertHeap(int taskIndex) {
    heap[heapSize] = taskIndex;
    heapifyUp(heapSize);
    heapSize++;
}

/*
 * extractMax:
 * pops and returns the index of the highest-priority task
 * NOTE: not currently used in menu, but kept for completeness
 */
int extractMax() {
    if (heapSize == 0) return -1;
    int topIndex = heap[0];
    heap[0] = heap[--heapSize];
    heapifyDown(0);
    return topIndex;
}

/* ---------------------- Core App Logic ---------------------- */

/*
 * addTask:
 * - asks user for title and priority
 * - creates a Task
 * - stores in tasks[]
 * - inserts the task index into the heap
 */
void addTask() {
    if (taskCount >= MAX_TASKS) {
        printf("Task limit reached! Cannot add more than %d tasks.\n", MAX_TASKS);
        return;
    }

    Task t;
    t.id = nextId++;

    printf("Enter title: ");
    fgets(t.title, sizeof(t.title), stdin);
    t.title[strcspn(t.title, "\n")] = 0; // remove newline

    printf("Priority (1=Low, 2=Medium, 3=High): ");
    if (scanf("%d", &t.priority) != 1) {
        // basic input safety
        printf("Invalid priority input.\n");
        // clear stdin
        while (getchar() != '\n');
        return;
    }
    getchar(); // eat leftover '\n'

    if (t.priority < 1 || t.priority > 3) {
        printf("Invalid priority. Must be 1, 2, or 3.\n");
        return;
    }

    t.done = 0;
    t.active = 1;

    tasks[taskCount] = t;
    insertHeap(taskCount);
    taskCount++;

    printf("✅ Task added with ID %d!\n", t.id);
}

/*
 * listTasks:
 * prints ALL active tasks, including done ones
 */
void listTasks() {
    printf("\n--- All Tasks ---\n");
    int found = 0;
    for (int i = 0; i < taskCount; i++) {
        if (!tasks[i].active) continue;
        found = 1;
        printf("[%d] %-25s | Priority: %d | %s\n",
               tasks[i].id,
               tasks[i].title,
               tasks[i].priority,
               tasks[i].done ? "DONE" : "PENDING");
    }
    if (!found) {
        printf("(No tasks yet)\n");
    }
}

/*
 * viewByPriority:
 * shows active, PENDING tasks based on heap order.
 * NOTE: we do NOT mutate the heap here; we just read it.
 */
void viewByPriority() {
    printf("\n--- Tasks by Priority (High → Low) ---\n");
    if (heapSize == 0) {
        printf("(No tasks in priority queue)\n");
        return;
    }

    int printed = 0;
    for (int i = 0; i < heapSize; i++) {
        int idx = heap[i];
        if (tasks[idx].active && !tasks[idx].done) {
            printf("[%d] %-25s | Priority: %d\n",
                   tasks[idx].id,
                   tasks[idx].title,
                   tasks[idx].priority);
            printed = 1;
        }
    }

    if (!printed) {
        printf("(No active pending tasks)\n");
    }
}

/*
 * showTopPriority:
 * shows the single highest-priority pending+active task (heap[0])
 */
void showTopPriority() {
    if (heapSize == 0) {
        printf("No tasks in queue!\n");
        return;
    }

    int idx = heap[0];

    if (!tasks[idx].active || tasks[idx].done) {
        // NOTE: We are not re-heapifying here.
        // In a future version we could clean inactive/done items from heap.
        printf("No active pending top task (heap needs cleanup).\n");
        return;
    }

    printf("\n--- Top Priority Task ---\n");
    printf("[%d] %s | Priority: %d\n",
           tasks[idx].id,
           tasks[idx].title,
           tasks[idx].priority);
}

/*
 * completeTask:
 * marks a task as DONE and pushes its ID onto the undo stack
 */
void completeTask() {
    int id;
    printf("Enter task ID to mark COMPLETE: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    getchar(); // eat '\n'

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].id == id && tasks[i].active) {
            if (tasks[i].done) {
                printf("Task %d is already completed!\n", id);
                return;
            }
            tasks[i].done = 1;
            pushUndo(id);
            printf("✅ Task %d marked as DONE!\n", id);
            return;
        }
    }
    printf("Task not found!\n");
}

/*
 * undoLastComplete:
 * pops from undo stack and marks that task as NOT done
 */
void undoLastComplete() {
    int id = popUndo();
    if (id == -1) {
        printf("Nothing to undo!\n");
        return;
    }

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].id == id) {
            tasks[i].done = 0;
            printf("↩ Undo: Task %d restored to PENDING!\n", id);
            return;
        }
    }

    printf("Undo failed: Task ID %d not found (should not happen)\n", id);
}

/*
 * deleteTask:
 * soft delete = mark active = 0
 * (we don't physically remove it from tasks[] or heap[])
 */
void deleteTask() {
    int id;
    printf("Enter task ID to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    getchar(); // eat '\n'

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].id == id && tasks[i].active) {
            tasks[i].active = 0;
            printf("🗑 Task %d deleted!\n", id);
            return;
        }
    }
    printf("Task not found!\n");
}

/* ---------------------- Menu / UI Layer ---------------------- */

void printMenu() {
    printf("\n=============================\n");
    printf("   TASK MANAGER (C Version)  \n");
    printf("=============================\n");
    printf("1) Add Task\n");
    printf("2) View All Tasks\n");
    printf("3) View Tasks by Priority\n");
    printf("4) Show Top Priority Task\n");
    printf("5) Complete Task\n");
    printf("6) Undo Last Complete\n");
    printf("7) Delete Task\n");
    printf("8) Quit\n");
    printf("Choose: ");
}

/*
 * menu:
 * classic infinite loop for the CLI app
 */
void menu() {
    int choice;
    while (1) {
        printMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            // clear stdin
            while (getchar() != '\n');
            continue;
        }
        getchar(); // eat '\n'

        switch (choice) {
            case 1: addTask(); break;
            case 2: listTasks(); break;
            case 3: viewByPriority(); break;
            case 4: showTopPriority(); break;
            case 5: completeTask(); break;
            case 6: undoLastComplete(); break;
            case 7: deleteTask(); break;
            case 8:
                printf("Goodbye!\n");
                exit(0);
            default:
                printf("Invalid option! Please choose 1-8.\n");
        }
    }
}

/* ---------------------- Entry Point ---------------------- */
int main() {
    menu();
    return 0;
}
