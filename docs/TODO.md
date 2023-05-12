# Testing 

- Tokenizer : impl scanner_cb_if and ensure all tokens scanned correctly
- List Parser : impl list_cb_if and ensure correct list building

# Benchmarking

Update the benchmark script to also generate the table we use to keep a log of times:

```
| test            | time (s)  | time (ms)
|----             |----       |----
| mandelbrot.nibi | 0.1575    | 157.5134
| primality.nibi  | 2.0588    | 2058.7793
| 4sieve.nibi     | 2.3497    | 2349.7038
| leibniz         | 0.1963    | 196.348
```

# Implementations

Consider trying out `https://github.com/faheel/BigInt` and run performance tests with it.
It would be nice to not be limited in integer size.


STD Lib:

```

First digit in arith determines expectations for numerical

(fn to_double [x] [
  (<- (+ 0.00 x))
])

```

## Importing and execution

```
 - Directory execution and tests with app.nibi
```

## Quoting 

```
  Right now we use [] () and {} to distinguish different types of lists, but under the 
  hood they are the same. 

  TO get the data is code is data relation we need a way to convert these list types to eachother
  and to convert them to their string representation, and then do evals of strings as if they were code
```

## Modules

### Reflection

Cell reflection, what more to say?

### Routers/ Channels

Potentially good for a module, a router or channel could be utilized to push
state changes and data to multiple functions