# Memory Allocation

- Give each env its own memory allocator so that way only the active environment scans 
its nodes for activity rather than the whole program. Then export the members to the parent 
memory manager if items are returned

- A "free cell queue" might be a good idea to pull cells from instead of performing allocations
upon request. would need an upper limit on it. Would also need more functionality in the language
than at the time of writing this to perform a memory profile test benefits.

- Assignments right now using `:=` rely on canning the current environment to ensure we don't trample a cell and clean it up before setting it. We could skip this if we give the cell a pointer to a `canary` of some sort (env etc) that way if they do trample it, the memory allocator could query the cell to ask it if its CERTAIN that its in use, and if its canary is gone it should set itself as not in use. Then we would only have to scan environments for existing items in global environments (because it wont go away so the canary wont die).

