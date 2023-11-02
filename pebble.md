
C-Types / ffi compat:

```
    C Type          Nibi Type
    char            byte
    uint8_t           u8
    int8_t            i8
    uint16_t         u16
    int16_t          i16
    uint32_t         u32
    int32_t          i32
    uint64_t         u64
    int64_t          i64
    void            none
    struct        struct
    * (ptr)         addr
```

Pebble doesn't need C-interop as its purpose is to be a compile-time execution engine
for the actual nibi compiler, but we need to consider C-interop as pebble will be
a subset of nibi.

Strings

Numbered structs will be the means by which we handle strings. A string such as "Hello"
will be composed into something such as:

```
; This syntax is not representative of nibi, but as an example of the structure


(let word :struct 
    (struct <some-unique-name>
        (member 0 72)
        (member 1 101)
        (member 2 108)
        (member 3 108)
        (member 4 111)
        (member 5 0)))

(put  word.0)    ; Prints '72'
(puts word.0)    ; Prints 'H'

(put  word)      ; Unknown at the time of writing
(puts word)      ; Prints "Hello"

```

Lists

Under the hood, lists themselves will be handled as structs in a similar manner.

```
(puts "Some text")
```

Will become a numbered struct, whose members structs themselves.

Given that these are structs, rather than a list of pointers to data in memory like a 
classic LISP, they can not be grown or expanded at runtime. This would be a severe limitation
for a lisp, but given the scope of pebble, we can get away with it. The proper implementation
of nibi may handle lists differently under the hood, though all interactions with the lists
available to pebble will also hold in nibi.

Boxes/ Bins/ Bowls/ ponds/ whatever they will be called

A reference counted, growable region of memory that can be used to store data.
These regions of memory aren't meant to be directly interpable with C, but rather,
are a means to collect/ store data that may need to be referenced in multiple functions.


Functions

All pebble functions' parameters will be passed by copy. Heap usage in pebble is 
intentionally limted, and passing references to stack locations can be a nightmare
if done wrong. Checking for reference passing and stopping mistakes is the job
of a compiler, of which pebble will be a _part_ of. If large data is being passed
by a function, its best to store it in a pond, and then pass a locator to the object(s).

