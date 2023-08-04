# Lists

In this language there are three types of lists. 

`()` denotes an instruction list
`[]` denotes a data list
`{}` denotes an accessor list

The instruction list is a list of instructions that gets executed.

Data lists are not explicitly executed as they "contain data" unless they
are used to denote a list of instructions for a given function. 

Accessor lists are used to index into environment cells similar to how in
some languages would do `something.function`

*Note*: Data lists that contain a symbol will be a sort of "reference" to the symbol
so when the item is accessed via `at` or `iter` the update will occur to the symbol
in question. If you wish to place a symbol into a list by value `clone` should be ran
to copy it into the list. With that said, since instructions inside data lists don't
get executed, in order to get the value in it must be done via a push, or by instantiating
the list with a default variable and then updated with `at` or `iter`.

# Quick Types

A quick type (qt) is a keyword that is bound to a specific value. These are created to make
programs more readable and to introduce some concepts that would otherwise have to be hand
coded in:

| keyword | value | description
|----     |----   |----
| nil     | nil   | A nil value
| true    | 1     | An integer with value 1
| false   | 0     | An integer with value 0
| nan     | NaN   | A float encoded with NaN
| inf     | inf   | A float encoded with inf

Any given qt can be used as a drop-in replacement for its raw data representation with the 
exception of `nil` which has no raw data representation, however, nil can be used as if it were 
any other piece of data in the instructions below.

# Instruction

| common commands | description | returns
|----     |----         |----
| :=      | Create or update a symbol in the immediate environment | cell that was assigned
| set     | Update an existing symbol in current or parent environment  | cell that was assigned
| drop    | Drop a symbol from current or parent environment | nil
| try     | Attempt to execute a list and handle any built in exceptions | last cell yielded from executed list
| throw   | Throw an exception that can be caught by `try` or will result in a interpreter halt | na
| assert  | Assert a given condition to be true or throw an error | nil
| len     | Retrieve the length of a string or list. Members of a different type will be stringed and measured | integer
| <-      | Return the value of a cell, yielding whatever execution may be happening | variable
| if       | If statement | variable
| loop    | A loop | variable
| clone   | clone a variable | variable
| fn      | Define a function | variable
| import  | Import files | 0
| use     | Use a module | 0
| exit    | Exit the program | N/A
| eval    | Evaluate a given string | variable
| quote   | Quote a a list into a string | variable
| nop     | Do nothing | nil
| macro   | Define a macro | variable
| dict | Create a dictionary | the new dictionary
| extern-call | Call a c-function from a shared library | variable
| alias   | Use a new symbol to refer to the data behing another | nil
| exchange | Update the value of a cell and return the old value of the cell | variable
| str-set-at | Update a string by inserting a value at a given index (negative indexing permitted) | updated string

| type commands | description | returns
|----   |---- |----
| type    | Retrieve a string detailing the type of a given item | string
| char    | Convert an integer into its char representation, or convert a string into a char type | char
| str     | Convert an item to a string type | converted value 
| int     | Convert an item to an integer type | converted value 
| i8      | Convert an item to the integer type | converted value 
| i16     | Convert an item to the integer type | converted value 
| i32     | Convert an item to the integer type | converted value 
| i64     | Convert an item to the integer type | converted value 
| u8      | Convert an item to the integer type | converted value 
| u16     | Convert an item to the integer type | converted value 
| u32     | Convert an item to the integer type | converted value 
| u64     | Convert an item to the integer type | converted value 
| float   | Convert an item to a float type | converted value 
| f32     | Convert an item to the float type | converted value 
| f64     | Convert an item to the float type | converted value 
| split   | Convert an item to a list comprised of the raw elements of the given variable | converted value 
| str-lit | Convert a list into a direct string rather than just a string representation | new string

| memory commands | description | returns
|----   |---- |----
| mem-new | Allocate some memory manually on the heap | Pointer cell|
| mem-del | Delete some memory on the heap | Pointer cell |
| mem-cpy | Copy a trivial cell's data to heap, or copy a pointer cell | Destination pointer cell |
| mem-load | Load a data from heap into a new cell | New cell of trivial cell type |
| mem-is-set | Check if a cell pointer has its pointer set to a space in memory | T/F |

| list commands | description | returns
|----   |---- |----
| >\|   | Push value to front of list | modified list cell
| \|<   | Push value to back of list  | modified list cell
| iter  | Iterate over a list         | iterated list
| at    | Retrieve an index into a list (negative indexing permitted | cell at given index
| <\|>  | Spawn a list of a given size with a given value | new list
| <<\|  | Pop front | list given sans the first element
| \|>>  | Pop back  | list given sans the last element

| arithmetic | description | returns
|---- |---- |----
| +  | Add a series of cells | resulting value  
| -  | Sub a series of cells | resulting value 
| /  | Div a series of cells | resulting value 
| *  | Mul a series of cells | resulting value 
| %  | Mod a series of cells | resulting value 
| ** | Pow a series of cells | resulting value 

| comparison | description | returns
|---- |---- |----
| eq  | Check for equality | returns numerical 1 or 0 representing true / false
| neq | Check for inequality | returns numerical 1 or 0 representing true / false
| <   | Less than | returns numerical 1 or 0 representing true / false
| >   | Greater than | returns numerical 1 or 0 representing true / false
| <=  | Less than or equal to | returns numerical 1 or 0 representing true / false
| >=  | Greater than or equal to | returns numerical 1 or 0 representing true / false
| and | Logical and | returns numerical 1 or 0 representing true / false
| or  | Logical or | returns numerical 1 or 0 representing true / false
| not | Logical not | returns numerical 1 or 0 representing true / false

| bitwise | description | returns
|----  |---- |----
| bw-lsh  | left shift  | integer  
| bw-rsh  | right shift | integer
| bw-and  | bitwise and | integer
| bw-or   | bitwise or  | integer
| bw-xor  | bitwise xor | integer
| bw-not  | bitwise not | integer

# Type prompting

For some commands expected type information is required. This is supplied to the
commands via the following "type tags."

| Type tag   | c type   |
|----        |----      |
| :u8        | unsigned 8 int |
| :u16       | unsigned 16 int | 
| :u32       | unsigned 32 int |
| :u64       | unsigned 64 int |
| :i8        | signed 8 int |
| :i16       | signed 16 int | 
| :i32       | signed 32 int |
| :i64       | signed 64 int |
| :f32       | 32-bit floating point |
| :f64       | 64-bit floating point |
| :str       | char* |
| :nil       | void  |
| :int       | signed 64 int |
| :float     | 64-bit floating point |
| :ptr       | void pointer |

All types listed as type tags has both a C, and a Nibi type corresponding to it. 
The Nibi types that have a direct C representation are known as "trival types" and
are mentioned throughout this document.

# Notation

**S** - A symbol (non keyword)

**[]** - Data list

**[*]** - Data list that will have its members executed

**()** - Processable list (Accessor lists `{}` are considered processable)

**<>** - A meta list to plug all accepted notation into a field

**..** - Indicating a continuation without specifed end

**STR** - An explicit `string` value

**I** - An explicit `integer` value

**RD** - Any raw data member (`string, integer, double`) 

**NU** - A numerical value (`integer, double`)

Example:

```

( keyword S < () .. S > )

Indicates that `keyword` explicitly takes a `symbol` first, and then can 
take a `processable list`, or a `symbol` as the second argument.

The meta list `<>` encompasses only one parameter into the given keyword

```


# Instruction Details

## keywords

### Alias

Keyword: `alias`

| arg 1 | arg 2 |
|----  |----
| item to alias | symbol to use as alias

```
  Note: Variables starting with `$` and `:` will not be allowed
        as they are reserved for error tokens and meta commands
```

```
( alias < () S [] RD >  < S > )
```

### Direct environment assignment

Keyword: `:=`

| arg 1 | arg 2 |
|----  |----
| symbol to designate | symbol or list that will yield the value to set

```
  Note: Variables starting with `$` and `:` will not be allowed
        as they are reserved for error tokens and meta commands
```

```
( := S < () S > )
```

### Indirect environment assignment

Keyword: `set`

| arg 1 | arg 2 |
|----  |----
| symbol or list that will yield the cell to update | symbol or list that will yield the value to set

```
( set < () S > < () S > )
```

### Drop assigned item

Keyword: `drop`

A variable number of arguments accepted into drop with the minimum being 1.

All arguments must be expressed as a `symbol`

```
( drop < S .. > .. )
```

### Retrieve length

Keyword: `len`

| arg 1
|----
| Item to retrieve length of 

```
Note: If the item is not a list it will be converted to a string and the length of the string
      value will be returned.
```

```
( len < RD S () [] > )
```

### Return item

Keyword: `<-`

| arg 1
|----
| Item to return

```
( <- < RD S () [] > )
```

### If / Else

Keyword: `if`

| arg 1               | arg 2                    | arg3 (optional)
|----                |----                     |----
| Condition to check | Body to execute if true | Body to execute if false

```
( if <() RD [*]> <() RD [*]> )
```

### Loop

Keyword: `if`

| arg 1          | arg 2               | arg3           | arg4
|----           |----                |----            |----
| Pre condition | Condition to check | Post condition | Body

Pre condition will be executed prior to the loop

Post condition will take place at the end of each loop

Anything declared within the loop will be scoped to the loop. This goes for anything done in 
the pre condition

```
( loop <()> <()> <()> <() [*]> )
```

### Clone

Keyword: `clone`

| arg 1
|----
| Item to clone

```
( clone <RD [] () S> )
```

### Function

Keyword: `fn`

All cells passed to a function will be done so by reference so they can
modify any given item.

To pass a an explicit copy use `clone` when calling the function on
the parameters to clone.

```
( fn <S> <[]> <() [*]> )
```

Anonymous functions can be created without provifind a `<S>` for a name.

```
( fn <[]> <() [*]> )
```

Variable number of args:

Using the `:args` tag as the only argument to a function, a list of arguments
of any size may be given. Example:

```
(use "io")

(fn vargs [:args] [

  (io::println "Called")

  (iter $args x [
    (io::print x)
  ])

  (io::println)
])

(vargs "a" "b" "c")
(vargs 1 2 3 4 5 6 7 8)
(vargs 1)
(vargs)
```

### Import

Keyword: `import`

Import files into the current operating environment as-is.

```
( import <S ...> )
```

All symbols in an import must be a string type, and not presented in any type of list.

The system will utilize any imports from `-i` or `--include` to look for files marked
to be included that are not immediatly present. The search priority is as follows:

1) The launch directory 

2) Include dirs as they appeared in order from include flag

3) The home directory under the directory `~/.nibi` iff the directory exists 

### Use

Keyword: `use`

Indicate that we want to use an installed, or relative module.

```
( use <S ...> )
```

Items brought in via `use` will be constructed in their own environment that must be
accessed with an accessor list unless that module specifies an export that ties a new
symbol to the inner items. See docs/example_modules for more information on module construction.

### Try

Keyword: `try`

Expects 2 parameters. The first will be executed and if an exception occurs
then the second list will be executed with a variable `$e` present in the 
environment while the recovery list is being processed.

Both parameters will execute top level data lists `[]` as a list of processable
lists, and execute each member.

```
( try < () [*] S > < () [*] S > )
```

### Throw

Keyword: `throw`

| arg 1 |
|----  |
| cell to execute and throw string value of

Note: Whatever is returned from the execution of arg 1 will be forcefully converted to a string for the given
exception. If the result can not be turned into a string it will throw a cell_access_exception instead.

```
( throw < [*] () S RD > )
```

### Assert

Keyword: `assert`

| arg 1 | arg 2 |
|----  |----
| condition to check | string value to throw in an exception if assertion fails <optional>

```
( assert < () S > STR )
( assert < () S > )
```

### Exit

Keyword: `exit`

| arg 1 |
|----
| Number to set as exit code - must be integer

```
( exit < NU S () >)
```

### Eval

Keyword: `eval`

| arg 1 |
|----
| String to evaluate

```
( eval < S STR > )
```

### Quote

Keyword `quote`

| arg 1 |
|----
| Item to quote

```
( quote < S () [] {} RD > )
```

### Str

Keyword `str`

| arg 1 |
|----
| Item to convert to string

```
( str < S () RD > )
```

### Int

Keyword `int`

| arg 1 |
|----
| Item to convert to an integer

```
( int < S () RD > )
```

### Float

Keyword `float`

| arg 1 |
|----
| Item to convert to a float

```
( float < S () RD > )
```

### Split 

| arg 1         | arg 2  
|----           |----
| Item to split | length - only required if arg 1 is a list

Note: Split will not resolve symbols in a given list

Note: A value of `0` passed as argument 2 when argument 1 is a list
      will result in a copy of the list being returned

```
( split < S () RD > )

# With list:

( split < S [] > < NU S () > )

```

### Type

Keyword `type`

| arg 1 |
|----
| Item to get the type of

Returns a string detailing the type of what `arg 1` resolves to.

The possible strings returned are as follows:

| return | info
|----    |----
| aberrant  | Aberrant cells are under-the-hood cells that shouldn't be able to be accessed |
| nil       | A nil cell        |
| u8        | unsigned 8 int |
| u16       | unsigned 16 int | 
| u32       | unsigned 32 int |
| u64       | unsigned 64 int |
| i8        | signed 8 int |
| i16       | signed 16 int | 
| i32       | signed 32 int |
| i64       | signed 64 int |
| f32       | 32-bit floating point |
| f64       | 64-bit floating point |
| ptr       | A pointer |
| string    | A string cell     |
| list:data     | A data list   |
| list:access   | An access list |
| list:instruction An instruction list |
| list          | Undetermined list type |
| function      | Function |
| environment   | An environment of cells |
| symbol        | A symbol  |
| dict          | A dictionary |
| unknown       | Unknown type |

The following cells will only show up if either something goes terribly wrong, 
or you are working on extending the language with a module or working on nibi itself.

    aberrant, list, symbol, unknown

```
( type < S () [] RD > ) 

```

### Nop

Keyword `nop`

No arguments given. Does nothing.

```
(nop)
```

----

## List Instructions

### Push Front

keyword: `>|`

```
Note: the `|` is meant to represent the boundary of a list, with the `>` showing 
      a direction that the data is being inserted. Notation assumes that the "front" of
      a list is the "left" side when reading
```

| arg 1 | arg 2 |
|----  |----
| list to push to | value to push

```
( >| < () [] S RD > [] )
```

### Push Back

keyword: `|<`

```
Note: the `|` is meant to represent the boundary of a list, with the `>` showing 
      a direction that the data is being inserted. Notation assumes that the "front" of
      a list is the "left" side when reading
```

| arg 1 | arg 2 |
|----  |----
| list to push to | value to push

```
( |< < () [] S RD > [] )
```

### Spawn list

keyword: `<|>`

| arg 1 | arg 2 |
|----  |----
| value to default all elements to| size of the list that must resolve to integer >0

```
( <|> < () S RD [] > < () I > )
```

### Pop front

| arg 1 |
|----
| list to pop the front of

```
( <<| < S [] > )
```

### Pop back

| arg 1 |
|----
| list to pop the back of

```
( |>> < S [] > )
```

### Iterate

keyword: `iter`

| arg 1            | arg 2                        | arg3 |
|----             |----                         |----
| list to iterate | symbol name to map value to | instruction(s) to execute per item

The symbol used in argument two will shadow any existing variable of the same name 
and map to whatever the value is of the list being iterated. At the end of the 
instruction, the symbol will be removed.

```
( iter < [] S > S < () [*] > )
```

### At

keyword: `at`

| arg 1               | arg 2 |
|----                |----
| list to index into | index to access

```
Note: Arg 2 is required to evaluate to an integer type
```

```
( at < S [] > < () S I > )
```

----

## Arithmetic

A variable number of arguments accepted with the minimum being 2, 
with the exception of `-` which will subtract the given value from 0 by default.

```
( op < S () .. > .. )
```

**Note:** `*` and `+` work on strings iff the first item given to them is a string.

----

## Comparisons

All comparison operators take exactly 2 arguments

`eq` and `neq` will accept any cell type and attempt to check equality.
Whatever the simple type the lhs takes on, eq and neq will attempt to convert the rhs to 
for checking. 

This means if the lhs operand is a string, it will attempt to stringify the rhs and so on
with integers and doubles. 

If the lhs is a more complex type like a list or a function, both sides will be 
stringified for comparison.

the remaining comparisons require that the arguments are numerical types (integer, float)

```
( op < S () > < S () > )
```

----

## Bitwise Instructions

### Left Shift

keyword: `bw-lsh`

| arg 1 | arg 2 |
|----  |----
| value to shift | amount to shift

```
( bw-lsh <() NU> <() NU> )
```

### Right Shift

keyword: `bw-rsh`

| arg 1 | arg 2 |
|----  |----
| value to shift | amount to shift

```
( bw-rsh <() NU> <() NU> )
```
### Bitwise AND

keyword: `bw-and`

| arg 1 | arg 2 |
|----  |----
| value to shift | amount to shift

```
( bw-and <() NU> <() NU> )
```
### Bitwise OR

keyword: `bw-or`

| arg 1 | arg 2 |
|----  |----
| value to shift | amount to shift

```
( bw-or <() NU> <() NU> )
```

### Bitwise XOR

keyword: `bw-xor`

| arg 1 | arg 2 |
|----  |----
| value to shift | amount to shift

```
( bw-xor <() NU> <() NU> )
```
### Bitwise NOT

keyword: `bw-not`

| arg 1
|----
| value to not

```
( bw-not <() NU> )
```

### Macro

keyword: `macro`

| arg 1 | arg 2       | arg 3 .. n |
|----   |----         |---         |
| name  | params list | macro body |

Example:

```lisp

# Create a macro that defines a `while` loop

(macro while [condition body]
    (loop (nop) %condition (nop) %body))

# Usable as :

(while (eq true true) (io::println "ayyy"))
```

The first argument of the macro is the name that can
be utilized like a function call to invoke it.

The second argument must resolve to a data list `[]`
and contain symbols that will be used to subsitute 
into the macro.

Every substitution of the word must be prefized with `%`.
These substitutions are 1-for-1 so all expected symbols
must fulfill syntax requirements for their position.

If a substitution symbol with the same name and form
as a parameter is to be omitted, prefixing it with `\`
will ensure that the symbol is ignored like so:

```lisp
(use "io")
(macro example [param] (io::println "Hey look its a: \%param"))
(example "nope")
```

will output the following:

    Hey look its a %param

## Dictionary

keyword: `dict`

The dict call takes a variable number of arguments depending
on the intended action.

Creating a dict:

```

# Variant 1 (empty)

(:= my_dict (dict))

# Variant 2 (empty)
(:= my_dict (dict []))

# Variant 3 (populated)

(:= my_dict (dict [
    [ "a" 33 ]
    [ "b" 44 ]
]))

# Variant 4 (populated)

(:= non_dict_list [
    [ "a" 33 ]
    [ "b" 44 ]
])

(:= my_dict (dict non_dict_list))

```

All dictionary keys MUST be literal strings, and the values can be anything.
Each entry in a list that constructs a dictionary must be a data list of size 2
with a literal string in slot 0, and the value in slot 1. 

Accessing a dict / creating entries:

```

# Create a new entry / overwrite existing
# without checking

(my_dict :let "key" some_value) # Anything is permitted in value

(set something (x :get "key"))  # Retrieve an existing value (throws if no-exist)

# Set something and ensure that it exists first

(set (x :get "key") "some new value")

# Delete a value

(x :del "key")

# Retrieve a list of keys

(:= the_keys (x :keys))

# Retrieve a list of values

(:= the_vals (x :vals))

```

As can be seen above, a created dictionary acts like a function for
accessing and updating with a command-like parameter that starts with a `:`

Dict commands:

| command | action
|---- |----
| :let | Create an item in the dict, will overwrite |
| :get | Retrieve a reference to a keys value |
| :del | Delete a key iff it exists. Returns 0 if key didn't exist, 1 otherwise |
| :keys | Get a list of keys |
| :vals | Get a list of references to values |

*Note:* The order that keys are printed or exist are not required to
fit any particular order. A list of values or keys retrieved may vary
in order.

Printing a dict:

Since dict symbols are essentially functions, stringing them has to be
done via a different method than `str`.

Calling a dict symbol with NO parameters as so:

```

    (my_dict)

```

will prompt it to return a fully formed, quoted string of the dict.

## External calls

Keyword: `extern-call`

Call and external library:

```


    (extern-call <library name> <function name> <[parameter c-types]> <return c-type>) 



    If library name is `nil` the symbol will be searched for based on 
    currently loaded symbols, otherwise it must be a bath to a library
    file - as a string.

    Note: If the extern method can not find the library it will attempt to
          locate it in the given include directories.
          Since the NIBI_PATH is added as an include directory,
          once can direct the cffi to an installed module's library
          file via "modules/<MODULE_NAME>/<LIB_NAME>"

    Function name is the function name as a string.

    Parameters is a data list of type tags (listed above)

    Return c-type is the c-type that represents the return
    type of the function

```

Examples:

```lisp
    (extern-cell nil "printf" [:str] :int "Hello, world!")
```

## Manual memory commands

Keyword: `mem-new`

Allocate a set sized amount of memory. 

Returns a `ptr` cell that points to the newly allocated memory.

If the argument is given `nil` instead of an integer, then the 
pointer returned will contain no allocated memory.

**Note about ptr cells:**

Pointer cells don't control the lifetime of the allocated memory. When the pointer cell
is deleted, the memory stays allocated. This would cause a memory leak. To prevent this
ensure you use mem-del on everything mem-new'd.

Pointer cells don't copy data when cloned to aother pointer cells. Both cells will point
at the same data in memory.

Keyword: `mem-del`

Delete the allocated memory. This function takes any number of arguments and attempts
to free them all. The last item freed will be returned.

Keyword: `mem-cpy`

Copy data in memory. If the first item is a standard "trival" cell. That is, one that has
a direct C-Type integration, it will be copied to the heap at a given destination pointer.

Example:

```lisp

(set x "Some string")
(set y (mem-new nil))

(mem-cpy x y)

# The same thing could be done as follows:

(:= y (mem-cpy "Some string" (mem-new nil)))

```

In the event that the destination value has already been allocated, that data will be automatically
freed before the copy takes place. 

Keyword: `mem-load`

Attempt to load a value from memory as a regular, trivial, cell. 

The load command requires the first argument to be a cell `type tag` (mentioned above) that will
indicate how much space to load from memory, and how to represent it as a cell.

```lisp

(:= result (mem-load :u64 my_ptr_cell))

```

Keyword: `mem-is-set`

Check if a ptr cell has been set, or if its not pointing to anything. Returns T/F

# Modules

```

module
    |
    |---- mod.nibi
    |
    |---- tests
    |       |--- test-feature-0.nibi
    |       |--- test-feature-1.nibi
    |
    module.lib
    file-0.nibi
    file-1.nibi
```

## mod.nibi

```
# These four settings are optional
(:= version "0.0.0")
(:= authors ["bosley"])
(:= description "User I/O extension module")
(:= licenses [
  "MIT"
])

# Load source files directly into an 
# environment
(:= sources [
  "file-0.nibi"
  "file-1.nibi"
])

# If a dynamic library needs to be loaded
# dylib can be set witha list of all functions
# that the library offers
# - Note: Dynamic libraries must take the name <module_name>.lib
(:= dylib [
  "get_str"
])

# optional list of files to execute in-order immediatly
# following an import. These can be used to redefine 
# access to the module, or run sanity checks before
# continuing.
# For instance we could export
# {math ceil} as math::ceil so access lists aren't 
# required for each call. 
(:= post [
  "export.nibi"
])

```

## Module tests 

Tests can be ran on installed moduled via:

`nibi -t <module_name>`

This will iterate over all detected test files and execute them
to ensure that the module is working correctly.

## Notes on modules

When a module is loaded, it is populated into its own environment cell.
This means any variables with a prefixed `_` will be private to the module, 
and accessing the module externally will be done via an accessor list `{}`.

Since all files within a module will be gathered into the same environment 
there is no need to `import` files internally, however, the environment will
be populated in the order that they are listed in `mod.nibi` so top level 
variables will only be aware of the things loaded before them. For function
definitions this is not an issue, as functions are not evaluated until they
are executed, but if any function is executed at the top-most level, and they
attempt to access a yet-to-exist cell there will be a runtime error.

## Module installation

Modules can be anywhere within the nibi include directories, but a module isn't 
considered installed until it exists in NIBI_PATH/modules.

# Applications

Applications are any direectory with a `main.nibi` within it. The distinction between that
and standard scripts is that applications can have a `tests` directory automatiocally discovered
and executed similarly to modules (above) and are to be launched at the directory level as so:

```
nibi ~/path/to/app
```

