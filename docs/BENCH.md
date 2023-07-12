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

### 15-May-2023

Removed global interpreter, and updated everything to take a reference to the interpreter. Some other 
changes here too but not to anything to do with processing (scripts/ modules/ etc). I believe the 
slight speed increase came from not having to jump to the heap to get the processor everytime
that a symbol needed to be processed.

git hash: **7b735129eb0e8ca15cee09ffe68d298bb57247d2**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1524s | 152.3795ms  |
| primality.nibi  | 1.7912s | 1791.2477ms |
| 4sieve.nibi     | 1.9994s | 1999.4224ms |
| leibniz.nibi    | 0.1703s | 170.2986ms  |

### 22-May-2023

Benchmark after adding eval and proc keywords - Didn't expect a change but wanted to
ensure slight interpreter changes didn't cause a slag

git hash: **c22ad54acc06c8651b73f3ebd3107a67c28fcfc4**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.181s  | 180.9553ms  |
| primality.nibi  | 1.7365s | 1736.5445ms |
| 4sieve.nibi     | 1.9878s | 1987.8231ms |
| leibniz.nibi    | 0.185s  | 184.9674ms  |

### 29-May-2023

Benchmark after changing out parser and tracking method

git hash **270588749191845e732b0d8c6c24eca3eb2b8dd3**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1453s   | 145.3236ms  |
| primality.nibi  | 1.8514s   | 1851.4003ms |
| 4sieve.nibi     | 2.1597s   | 2159.7168ms |
| leibniz.nibi    | 0.182s    | 182.0244ms  |

### 05-June-2023

Removal of proc made us have to clone a few more times and pass env to clone so 
a slowdown was experienced.

git hash **82465958f327a7e17484f2db38125a813c476ca2**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.2475s   | 247.4568ms  |
| primality.nibi  | 2.0008s   | 2000.759ms  |
| 4sieve.nibi     | 2.5767s   | 2576.7131ms |
| leibniz.nibi    | 0.2387s   | 238.727ms   |

Removal of proc and introducing this slight hit was to remove the symbol
passing behavior that won't make sense when the language becomes compiled
to bytecode. So while it hits the raw list walking, it will benefit in the 
long run.

### 25-June-2023

Removal of env command and some refactoring of how interpreter is passed around.

git hash **be2acfa80c45e75b0fb715fd1bd76d59bce5b502**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1108s   | 110.8305ms  |
| primality.nibi  | 1.0571s   | 1057.0616ms |
| 4sieve.nibi     | 1.2172s   | 1217.2355ms |
| leibniz.nibi    | 0.1192s   | 119.2178ms  |

### 28-June-2023

Added macros and started making nibi pull in "standard" imports on startup.

git hash **30ca5c4c0913ca7171a8f323510dbefee1c8ece5**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1122s   | 112.1821ms  |
| primality.nibi  | 1.055s    | 1054.9819ms |
| 4sieve.nibi     | 1.2322s   | 1232.1547ms |
| leibniz.nibi    | 0.1223s   | 122.2559ms  |

Thankfully no slow downs at all :-)

### 03-July-2023

Added call stack recording and fixed a function clone issue.

git hash **74da7ad78d57ec956d0081827fab7fffbd32609c**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1176s   | 117.6493ms  |
| primality.nibi  | 1.0805s   | 1080.4997ms |
| 4sieve.nibi     | 1.3043s   | 1304.3069ms |
| leibniz.nibi    | 0.1247s   | 124.6889ms  |

Minor improvements

git hash **10561b9da5c80c5996b8fd5a05eb4084e493b9b7**

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1148s   | 114.8487ms  |
| primality.nibi  | 1.076s    | 1076.0419ms |
| 4sieve.nibi     | 1.2633s   | 1263.2533ms |
| leibniz.nibi    | 0.1215s   | 121.548ms   |

### 08-July-2023

Map/ Environment opts

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.0899s   | 89.923ms    |
| primality.nibi  | 0.9276s   | 927.5601ms  |
| 4sieve.nibi     | 1.0s      | 1000.0357ms |
| leibniz.nibi    | 0.1002s   | 100.1686ms  |

Full removal of phmaps and testing each map for most 
efficient runs against test_perfs:

| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.0843s   | 84.2987ms  |
| primality.nibi  | 0.8581s   | 858.0874ms |
| 4sieve.nibi     | 0.9207s   | 920.7101ms |
| leibniz.nibi    | 0.0967s   | 96.7408ms  |

### 11-July-2023

Setup a `#define` to switch beteween using a std::vector
and a std::deque. The std::vector needed a couple of
helpers to do some operations, but they were _somewhat_
better in performance than the deque.

Until language is hashed out entirely this will remain
to switch back and forth for performance tests in 
the interpreter.

 test             | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.0822s   | 82.2418ms  |
| primality.nibi  | 0.8339s   | 833.8564ms |
| 4sieve.nibi     | 0.8865s   | 886.4585ms |
| leibniz.nibi    | 0.0944s   | 94.4019ms  |
