# Testing 

- Tokenizer : impl scanner_cb_if and ensure all tokens scanned correctly
- List Parser : impl list_cb_if and ensure correct list building
- GC / Mem : Test garbage collector

# Implementations

`:=` Assignment operator
cell clone method so we can do a deep copy from cell to cell
`true!` as the first and base assertion so we can make scripted tests
`false!` ^^ 



- Lists are being populated, and _some_ builtins are mapped but not implemented. need to impl them and add other builtins



Keyword `ref` should be used to _not_ clone an item, and assume any (:= a b) is an indication to clone b into new symbol a




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