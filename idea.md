```

#         comment
()        list to execute
{}        a 'body' of lists to execute that allows parameters
[]        create a list of data
...       variadic arguments
fn        create a function
loop      a loop structure
if        an if statement
:=        assignment
<-        return value
len       return the length of a list
at        returns a reference to an item within a list, returns nil if oob
view      a view into a list, returns reference of list
quote     stringify the parameters as a list
eval      evaluate a string
drop      drop a variable from its scope
type      retrieve the type of a given variable
quit      stop all processing
rev       reverse a list and return a reference to it
sub       parallel subroutine - returns promise
await     await the result of a promise
use       incorperate a file
load      load an extension or compiled tokra bin
nil?      check to see if item is nil returns true iff item is nil
promise?  check to see if item is promise returns true iff item is promise
exists?   check to see if given item(s) exist within scope
$args     the arguments for the current function as a list
params    bind variables to their relative place in the `$args` list
mod       wrap statement(s) in a module name
pub       make items available externally to something wrapped with mod
::        access item within a mod : `mod_name::public_item`
+         add
-         subtract
/         divide
*         multiply
**        power
%         modulus
<<        left shift
>>        right shift
&&        logical and
||        logical or
!         logical not
&         bitwise and
|         bitwise or
~         bitwise not
^         bitwise xor
```


### Idea example modules

(mod std {
  (pub {
    (:= car (fn (x) (<- at 0 x)))
    (:= cdr (fn (x) (view x [1 (len x)])))
    (:= cons (fn (...) (<- $args)))
  })
})

(std::car [1 2 3 4])  # Returns 1
(std::cdr [1 2 3 4])  # Returns [2 3 4]
(std::cons 1 2 3 4)   # Returns [1 2 3 4]



