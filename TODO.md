# Testing 

- Tokenizer : impl scanner_cb_if and ensure all tokens scanned correctly
- List Parser : impl list_cb_if and ensure correct list building
- GC / Mem : Test garbage collector

# Implementations

- Start populating the lists returned by list parser into an instruction set

- Rename compiler to "front" or "intake" or "setup" ... or "shared" ... "common" ?

- Start a processor that impls the list_cb_if that processes the lists as they come in
  need to work in the mark and sweep of these instructions as they come in. We don't 
  need to have all instructions allocated throughout the lifetime of the program, which
  means we don't want be referencing data in instructions within the env.

  This means we should impl a "clone" in cell to do a deep copy of a cell and its data. 
  So we can pull it from the instruction. THAT or we mark the instructions whose 
  data we rely on as in use.

  We've been operating on the assumption all envs will have their owm memory boyo
  but we _could_ have 1 for everything, and intermittently have all envs mark cells.
  This means for reference cells and whatnot we should have a means to propagate
  the marks to children

- Lists are being populated, and _some_ builtins are mapped but not implemented. need to impl them and add other builtins



Keyword `ref` should be used to _not_ clone an item, and assume any (:= a b) is an indication to clone b into new symbol a

