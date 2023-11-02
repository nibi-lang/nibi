

First stop:

Bytecode interpreter of minimal list processing engine "pebble"

The smallest most light-weight nibi.
    - basic control flow
    - strong c-types
    - no arrays or data "lists", but structs.
      some structs will be auto generated (strings will become a struct of size (len * byte) where 
      each element of the string is named after its index)
      lists themselves will also be represented as numbered structs.
    - no "real" macros. There 'might' be some processor directives that permit
      stupid code copy/completion to make early development easier.
        > `pebble` the interpreter will be the compiler's `compexpr` engine that 
          handles compile-time code execution and the macro system
    - simple reference-counted shared objects (boxes, bins, or some such) that won't 
      be anything more than simple heap storage. No raw pointers, etc.

Second stop:

Write a proper, minimal compiler for pebble, using the interpreted pebble engine as a processor within 
the compiler for compile-time execution (`compexpr` lists). This will be the root of the 
"macro" system.

This stage-0 compiler will introduce cffi compatability, heap management, etc.
This first stage-0 will add the minimum set of functionality required to bootstrap.

Third stop:

Rewrite the pebble interpreter in nibi. This rewrite should produce a nibi "bucket",
a library containing nibi code. At this point we _may_ write a repl in nibi using the bucket.

Fourth stop:

The stage-1 compiler. Using the stage-0 version of nibi, and the pebble bucket for the
execution of `compexr`, we will write the nibi compiler _in_ nibi.

Fifth stop:

Recompile the pebble bucket with the new stage-1 compiler, and then re-build the stage-1 compiler
with the new pebble bucket.

This might not warrant a whole stage if not for the fact that its a milestone. A big one.


