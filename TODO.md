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

------------------------

Importing and execution

```

- Launching cli against file pulls in file and executes. in the future may or may not convert to bytecode first

- Launching cli against a directory, where an app.nibi will be read for application settings and information 
  including include dirs, entry file, and test file listing so nibi -t dir/ can be ran and have all `.test` 
  files executed in the order they are listed.


(import "file.nibi" ... )

Will import the files in the given order - populating the environment with whatever they contain in the top level

using (import "some_dir") will attempt to load the item as a directory for grouping imports (lib.nibi) will
be required in this case.

```



------------------------

(object x
  (fn __init__ () [])

  (:= member_data 0)
  (fn member_method () [

  ])
)

(:= item (x))

(item.member_method )


------------------------

### Custom interfaces/ types - Later Potential implementations

```
iface - define interface
fn    - Describe a function that will be implemented
var   - Insist a variable exists

(iface my_interface 
  [
    (fn my_method (arg1 arg2 arg3))
    (fn my_other_method)
    (var x)
  ]
)


---- Custom type -----

(obj my_type [

  // No constructor - if the user wants they can pass a list to the type
  // on creation and it will be mapped to variables in order

  (pub fn some_method (a b c) [])

  (fn some_private_method [])

  // Var is used to describe that a given var will exist
  (var some_variable)
])

// Example instantiation -
// the my_type becomes a function that returns a cell containing the type 
// and instantiated with whatever data list is given
(:= x (my_type [0]))


(type my_type my_interface [

  // MUST implement all items in interface
])


```

### Routers

A router as a built in type that can be defined to take a particular interface 
in to register to recieve data, and others can call on routers to transmit data


(router some_name target_interface)
(router some_name object_type)

(some_name register x)

  router    router fn   obj fn
(some_name    submit    get_data   "Data to go to registered interfaces")



```



```