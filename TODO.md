# Testing 

- Tokenizer : impl scanner_cb_if and ensure all tokens scanned correctly
- List Parser : impl list_cb_if and ensure correct list building
- GC / Mem : Test garbage collector

# Implementations


- Start a processor that impls the list_cb_if that processes the lists as they come in
  need to work in the mark and sweep of these instructions as they come in. We don't 
  need to have all instructions allocated throughout the lifetime of the program, which
  means we don't want be referencing data in instructions within the env.

  This means we should impl a "clone" in cell to do a deep copy of a cell and its data. 
  So we can pull it from the instruction. THAT or we mark the instructions whose 
  data we rely on as in use.


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