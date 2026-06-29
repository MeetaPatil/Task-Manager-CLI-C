# Task Manager (CLI) — C

A terminal-based task manager written in C, demonstrating practical use of core data structures: an **array-backed task store**, a **max-heap priority queue**, and a **LIFO undo stack**.

---

## Features

| Feature | Description |
|---|---|
| Add Task | Create a task with a title and priority (Low / Medium / High) |
| View All Tasks | List every active task and its current status |
| View by Priority | Show pending tasks ordered High → Low using a max-heap |
| Top Priority Task | Peek at the single highest-priority pending task |
| Complete Task | Mark a task as done |
| Undo | Revert the last completion (stack-based, LIFO) |
| Delete Task | Soft-delete a task (hides it without removing from memory) |

---

## Build & Run

**Requirements:** GCC (or any C99-compatible compiler)

```bash
# Compile
gcc task_manager.c -o task_manager

# Run
./task_manager
```

---

## Usage

On launch you'll see a numbered menu. Type a number and press Enter.

```
=============================
   TASK MANAGER (C Version)
=============================
1) Add Task
2) View All Tasks
3) View Tasks by Priority
4) Show Top Priority Task
5) Complete Task
6) Undo Last Complete
7) Delete Task
8) Quit
Choose:
```

### Adding a task

```
Enter title: Fix login bug
Priority (1=Low, 2=Medium, 3=High): 3
✅ Task added with ID 1!
```

### Completing & undoing

```
Enter task ID to mark COMPLETE: 1
✅ Task 1 marked as DONE!

# Undo it:
↩ Undo: Task 1 restored to PENDING!
```

---

## Data Structures

### Task Array (`tasks[]`)
A fixed-size array of `Task` structs acts as the in-memory store. Each task holds an ID, title, priority, done flag, and active flag.

### Max-Heap (Priority Queue)
The `heap[]` array holds *indices* into `tasks[]` and is maintained as a max-heap keyed on `priority`. This allows O(log n) inserts and O(1) peek of the highest-priority task.

### Undo Stack (`undoStack[]`)
A simple integer stack that records the IDs of recently completed tasks. Calling **Undo** pops the top ID and flips that task back to pending — O(1) push and pop.

---

## Configuration

| Constant | Default | Description |
|---|---|---|
| `MAX_TASKS` | `100` | Maximum number of tasks |

Change `MAX_TASKS` at the top of `task_manager.c` and recompile to adjust capacity.

---

## Known Limitations

- **No persistence** — all data lives in memory and is lost on exit.
- **Heap not cleaned up** — soft-deleted or completed tasks remain in the heap until the program restarts; `showTopPriority` may report a stale result if the top entry was deleted.
- **Fixed title length** — task titles are capped at 49 characters.

---

## Potential Improvements

- [ ] Save/load tasks to a file (CSV or binary)
- [ ] Rebuild heap on startup after loading saved data
- [ ] Remove completed/deleted entries from the heap lazily (`heapify` on extraction)
- [ ] Multi-level undo (currently only one level deep via the stack)
- [ ] Edit task title or priority after creation
- [ ] Filter view by status (pending only / completed only)

---

## Authors

- Neil Sharma
- Meeta Patil
- Siddhi Sawant

---

## License

Free to use and modify for personal or college projects. Credit appreciated but not required.
