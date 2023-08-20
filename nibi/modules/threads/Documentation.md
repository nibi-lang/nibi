# Module: threads

A threading module

| method  | exported as    | parameters | returns
|----     |----            |----        |----
| nibi_threads_future | threads::future | list | integer thread id
| nibi_threads_future_get | threads::future::get | thread id | resulting value, or nil if not ready
| nibi_threads_future_wait_for | threads::future::wait | thread id, microseconds | integer (ready, timeout, unknown)
| nibi_threads_future_is_ready | threads::future::ready | thread id | integer 1 = ready
| nibi_threads_future_runtime  | threads::future::runtime | thread id | get ms runtime
| nibi_threads_future_kill     | threads::future::kill | thread id | integer 1 = killed
| nibi_threads_sleep           | threads::sleep | microseconds | parameter in unmodified
| nibi_threads_active          | threads::active | N/A | number of active threads from module

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

### Notes

When the user executes `get` on a future, and the valus is returned as the future is completed,
that thread will then be cleaned up. Running `get` on a future that has already been obtained
will cause the same result as calling `get` on an id that is invalid; an interpreter exception
will be raised.
