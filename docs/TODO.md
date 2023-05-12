# Testing 

- Tokenizer : impl scanner_cb_if and ensure all tokens scanned correctly
- List Parser : impl list_cb_if and ensure correct list building

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

## Modules

### Debug

Remove the @debug and other dbg related commands, make it a module

This will lessen the overall map size for keywords 

### Reflection

Cell reflection, what more to say?

### Routers/ Channels

Potentially good for a module, a router or channel could be utilized to push
state changes and data to multiple functions