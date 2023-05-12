## bosleyslab

### 5-5-2023

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

```
<< BENCH >>

Binary:  ../build/nibi
N Executions per benchmark:  50
CWD :  /home/bosley/workspace/nibi/test_perfs

------- BEGIN BENCHMARK -------

Scanning directory :  /home/bosley/workspace/nibi/test_perfs

Running: /home/bosley/workspace/nibi/test_perfs/mandelbrot.nibi
Expecting return code:  0
Average execution time:  0.1731s ( 173.108ms )

Running: /home/bosley/workspace/nibi/test_perfs/primality.nibi
Expecting return code:  0
Average execution time:  2.1617s ( 2161.7256ms )

Running: /home/bosley/workspace/nibi/test_perfs/4sieve.nibi
Expecting return code:  0
Average execution time:  2.4722s ( 2472.1891ms )

Running: /home/bosley/workspace/nibi/test_perfs/leibniz.nibi
Expecting return code:  0
Average execution time:  0.2393s ( 239.2551ms )


-------- END BENCHMARK --------
```