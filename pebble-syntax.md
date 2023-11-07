Note: This describes basics of the pebble "language subset."
More functionality may be installed by the nibi compiler as "Extensions"
that allow interop with the compiler system itsself that will be unique
to the compile-time system and not exist within the nibi language 
outside of `compexpr`s

The scope of pebble limits a lot of things you'd expect in a fully featured
language like fileio/ userio/ networking/ etc. These will be added to
nibi proper.




Types / Declarations

Types of objects may be implicit, but they can also be explicit.

To enforce a type, we use "type tags" that take the form `:tag-name`.
For each built-in type there is a tag, and for each user-defined type,
a tag will be made.

Built in types:

```
    byte
      u8
      i8
     u16
     i16
     u32
     i32
     u64
     i64
    none
  struct
    addr < Excluded by pebble, but is part of nibi >
```




Using types/ making vars

```

(let x :i32 42)
(set x (+ x 1))

(let y :real 3)     ; Converts to real: 3.0
```

The following block is how addresses/pointers will be utilized, but
it _may_ not be available in `pebble`, but rather, in `nibi` proper. 
Adding it to pebble as an explicit option by the developer may be done later.
```
(let y :real 3)     ; Converts to real: 3.0
(let z :addr ($ y)) ; z is now an address for y that can be passed around.

(put z)     ; Prints the address 0x00...

(put (@ z)) ; Prints `3.0`
```




Structs

```
; Defining a struct

(struct coord
    x :u64
    y :u64)

; Setting a struct

(let start :coord       ; coord has become a type-tag
    :x 0                ; Scoped parameter assignment (required)
    :y 0)

(put start.x)           ; Access
```

Numbered-Structs

Taking a similar form as structs, numbered structs use unsigned integers
as the member names. These structs can not be created by the user explicitly.
They are specialized structs generated by the interpreter/compiler used
to represent data.

Strings

```

(let name :str "Bosley")

; Strings become a numbered struct where each member 
; takes the number that is the index of a part of a string,
; and whose value will be a `byte` containing the individual
; string point


(put    name.0)    ; Prints the numerical value of the first element
(puts   name.0)    ; Prints the 'B' representation

```




Functions

All executable code must reside in functions. When used as the compile-time
vm, pebble will have an "implicit function" that everything will be executed within.
This means we can have functions within function definitions, that will be explicitly
scoped to the parent function.

```
            Return type is BEFORE params
                |
(fn add-three :i8 (a :i8 b :i8 c :i8)
    (+ a b c))  ;   <- Implicit returns on last executed list

(fn add-three :i8 (a :i8 b :i8 c :i8)
    (ret 0)         ; <- Explicit early return
    (+ a b c))

```

Given the scope of pebble, it won't be taking any input arguments on execution.
The types required to pass lists and tag them with types won't be available until later,
in the full nibi implementation. 




Scopes

Defining named scopes helps with naming and organization, so it can be done as such:

```

(scope math

    (let pi :real 3.14159)

    (fn add :u64 (lhs :u64 rhs :u64)
        (+ lhs rhs))
)

(puts math.pi)

(puts (math.add 10 4))


; Scopes will stay alive within their function unless they are made to be anonymous.
; Anonymous scopes, when passed, evaporate and everything defined within will be
; removed. 

(scope _

    (let myscopedvar 3)
    ...
    ...

)

; myscopedvar is now gone

```




Control Flow

Again, because of the limted scope of pebble, we will potentially be missing some things.

If-Expression

Ifs will be expressions that return values. These values CAN be ignored, or they can
be utilized. 

```

(let x 0)
(let y 1)

(if x
    (puts "X was true)
    (puts "X was false))

; The first list appearing will be the true body of the 'true' case, with the
; second being the 'false' body. 

```

If more than one list is required to be executed in a condition, using `body` will
permit the execution of multiple lists sequentially as such:

```

(if x 
    (body           ; the 'true' body
        (puts....)
        (puts....))
    (puts "It was false, jim")
)

```

To leave the if body with an explicit value, the `<-` operator can be utilized
in the last occurring list. If an if statement has a `<-` present, both of the
bodies (true and false) MUST return the same type _explicitly_.




Loops

The only loop available in pebble will be `do`. Other, fancier loops, will
be available in nibi based on macros made in pebble utilizing do.

```
; Infinite loop saying hi!
(do 
    (puts "Hi!"))
```

Things defined within the `do` loop will remain after the completion of
the loop. If this is not wanted, an anonymous scope may be used.

Getting out of nested loops : scope breaking

Sometimes we want to leave a particular section of code without it being tied to
a function. In some languages they have "named loops." In nibi/pebble we use scopes
to break out of.

```
(scope _
    (scope outer
        (do
            ...
            ...

        (scope inner
            (do
                ...
                (if x (break outer))        ; Leave the upper-most scope if x is true
                ...
                ...)))
    )
)

; Since the named scopes were wrapped in an anonymous scope, everything, including the 
; named scopes are now gone.
; Without doing so, the named scopes would linger around "stale scope"
```




Matching (switch/case)

```
(match x
    (10 (puts "its 10"))
    (20 (puts "its 20"))
    (_  (puts "Its something else")))    ; Default/ final case
```

Similar to the if-expression, the match can also return a specific falue with the `<-` operator.
Also similar to the if-expression, if `<-` exists in any list, they must be in all lists.

Note: In the future, in nibi proper, we may add a type tag to match which _could_ then permit
the implicit cast of any return to a type allowing the user to use `<-` in fewer cases than all of
them explicitly.




Comparison operations

```
lt
gt
lte
gte
eq
neq
```

Logical operations

```
and
or
xor     ; Logical exclusive or
```

Bitwise operations

```
lsh
rsh
bw-or
bw-and
bw-not
bw-xor
```



Uses within nibi (ideaing again)


```
(compexr        ; Invoke the compile-time execution of this list
                ; > May end up using `{}` lists as shorthand for this

    (scope app-info

        (struct semver
           major :u32
           minor :u32
           patch :u32)

        (let version :semver 
            (semver (
                ::major 0
                ::minor 1
                ::patch 0)))

        (let hash :str compiler.githash)
        (let date :u64 compiler.datetime)
    )
)
```

Compexrs will be executed as they are detected (in-order of inclusion.)
The above, if appearing early in the program, would define the app-info scope
that could be used in actual nibi code, where the data will be placed where
accessed within nibi.

Nibi will be able to retrieve data from something defined in a constexpr, but
constexprs can not take in variables or information from actual nibi code where
the data wouldn't be available until runtime (user io/etc.)


```
; Nibi code, not pebble, but _using_ pebble

(use io)
(fn main :u8 (args :list)
    (io.println 
        "Build hash: " 
        (compexpr (<- app-info.hash))
        ))
```