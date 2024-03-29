# Module: threads

A threading module

| method  | exported as    | parameters | returns
|----     |----            |----        |----
| nibi_threads_fn | threads::fn | list of params | variable
| nibi_threads_future | threads::future | list | integer thread id
| nibi_threads_future_get | threads::future::get | thread id | resulting value, or nil if not ready
| nibi_threads_future_wait_for | threads::future::wait | thread id, microseconds | integer (ready, timeout, unknown)
| nibi_threads_future_is_ready | threads::future::ready | thread id | integer 1 = ready
| nibi_threads_future_runtime  | threads::future::runtime | thread id | get ms runtime
| nibi_threads_future_kill     | threads::future::kill | thread id | integer 1 = killed
| nibi_threads_sleep           | threads::sleep | microseconds | parameter in unmodified
| nibi_threads_active          | threads::active | N/A | number of active threads from module
| nibi_threads_shutdown        | threads::shutdown | N/A | 1 when complete. All running threads will be killed and cleaned.

### wait results

| symbol            | meaning
|----               |----
| threads::future::status::ready | item is ready to get
| threads::future::status::timeout | wait time has been exceeded, item not ready
| threads::future::status::unknown | an internal error has occured and the status could not be retrieved

### time

| symbol            | meaning
|----               |----
| threads::time::seconds | Returns number of seconds handed in as microseconds
| threads::time::milli   | Returns number of milliseconds handed in as microseconds

## Notes

### Thread trampling

Threads are only meant to be run within the boundary of a `threads::fn`. While
reading from the external environment is usually fine, calling external functions
from multiple threads within the same environment can cause crashes. Regular functions in
nibi are not just definitions of a function, but the actual implementation of the function as well.
This means that if two threads attempt to execute a function at the same time, they will trample
each other. There is a demo of this in the `programs` directory. To ensure this doesn't happen
the programmer should do all actions within the scope of the `threads::fn` as this type of function
when used with `threads::future` makes unique implementations of the called function. 

In the future we may add specific types that ensure atomic access to data.

### Getting futures

When the user executes `get` on a future, and the valus is returned as the future is completed,
that thread will then be cleaned up. Running `get` on a future that has already been obtained
will cause the same result as calling `get` on an id that is invalid; an interpreter exception
will be raised.

### Handles and memory

Executing threads and not keeping a handle on them can lead to futures hanging out in space keeping their
memory around until program end of life. This is technically a memory leak. The only time threads
_should_ be spawned and never having `get` ran on them is if they are meant to exist for the lifetime of
the program.

In any event, at the end of a program using this module `threads::shutdown` should be executed to kill
off and clean up any remaining threads whose handles may have been discarded.

It is also important to note that once shutdown is executed, the next call into threads will restart the
required data structures to function and shutdown can be used again later to clean up again.
