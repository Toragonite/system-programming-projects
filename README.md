# System Programming — course projects

Sogang University, System Programming (Spring 2024, Prof. Youngjae Kim). Individual projects in C.

| Folder | Project | What it does |
| --- | --- | --- |
| `project1-mylib/` | Project 1 — `mylib` | Interactive tester for the Pintos kernel data structures: doubly linked list, hash table and bitmap, driven by a small command parser. |
| `project3-stock-server/` | Project 3 — concurrent stock server | A stock trading server (`show`, `buy`, `sell`) built twice and compared: event-driven with `select()`, and thread-based with a worker pool. |
| `project4-malloc/` | Project 4 — dynamic memory allocator | `malloc` / `free` / `realloc` on segregated free lists. |

Project 2 (shell) is not in this repository. `csapp.c`, `csapp.h` and the small client programs in Project 3 come from the CS:APP course materials; the servers, the stock tree and the allocator are mine.

## Project 3 — concurrent stock server

- **Stock table**: a binary search tree keyed by stock ID, loaded from `stock.txt` at start and written back on `SIGINT`.
- **`task1/` event-driven**: one process multiplexes all clients with `select()` over a pool of connected descriptors.
- **`task2/` thread-based**: a master thread accepts connections and hands them to worker threads through a bounded shared buffer guarded by semaphores. Each stock node carries its own readers-writers semaphores, so `show` requests read concurrently while `buy` and `sell` write exclusively.
- **Evaluation**: total handling time measured from 1 to 1,000 concurrent clients for the event-driven server and for pools of 500 and 1,000 threads, with 10 to 80 stocks.

```bash
cd project3-stock-server/task1   # or task2
make
./stockserver <port>
./multiclient <host> <port> <clients>
```

## Project 4 — dynamic memory allocator

- 10 segregated free lists by size class, with the list heads stored inside the heap rather than in a global array.
- First-fit search within a class, block splitting on placement, and boundary-tag coalescing on free.
- `realloc` grows in place when the neighbouring block is free and otherwise allocates and copies.
- The heap is extended only by the amount still missing when the last block is free.

`project4-malloc/mm.c` plugs into the CS:APP malloc-lab driver (`mdriver`), which is not included here.
