## bosleyslab

### 5-May-2023

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1407    | 140.6523
| primality.nibi  | 1.5692    | 1569.1921
| 4sieve.nibi     | 1.9182    | 1918.2287

**Notes:** 
```
These tests were written today and this is the final bench following optimization work.
Originally the cell_list_t was a std::forward_list but with only changing the underlying 
type to be a std::deque the 4sieve bench went from taking 60 seconds to its current value.

Some other optimizations were put in place to get the overall numbers up, but it appears
that Nibi is already much faster than the current Sauros impelemntation:

mandelbrot.saur 0.4771s ( 477.0854ms )
primality.saur  2.4532s ( 2453.189ms )
4sieve.saur     2.1253s ( 2125.2623ms )
```

### 11-May-2023

More than minimal implementation completed. Now marks the start of being able to import
files and modules. Tests running at the time of writing this - I suspect things will be 
somewhat slower now that the interpreter has expanded. Optimization work will begin
once directory execution and a few other things are complete.

git hash: **816888e782f214d405c083a511d0b057bf2f1db4**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1575    | 157.5134
| primality.nibi  | 2.0588    | 2058.7793
| 4sieve.nibi     | 2.3497    | 2349.7038
| leibniz         | 0.1963    | 196.348
