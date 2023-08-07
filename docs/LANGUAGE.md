Lists in this programming language are classified into three types, each represented by different symbols:

1. **Instruction Lists:** Denoted by `()`, these lists contain a sequence of instructions that are executed in order.

2. **Data Lists:** Denoted by `[]`, these lists store data and are not executed directly unless they are used to represent a list of instructions for a specific function.

3. **Accessor Lists:** Denoted by `{}`, these lists are used to index into environment cells, functioning similarly to accessing elements in other programming languages.

Important Notes:
- Data lists containing symbols act as references to the symbol, allowing updates to occur when accessed via `at` or `iter`. To place a symbol into a list by value, use the `clone` function to make a copy.
- Instructions inside data lists are not executed directly. To access their values, you can use `push`, instantiate the list with a default variable, and then update it with `at` or `iter`.


# Quick Types

In this programming language, Quick Types (qt) are special keywords that represent specific values. These keywords are designed to improve program readability and introduce predefined concepts, eliminating the need for manual coding.

| Keyword | Value | Description               |
| ------- | ----- | ------------------------- |
| nil     | nil   | Represents a nil value    |
| true    | 1     | Represents an integer with value 1 |
| false   | 0     | Represents an integer with value 0 |
| nan     | NaN   | Represents a float encoded with NaN |
| inf     | inf   | Represents a float encoded with infinity |

Quick Types can be directly used in place of their corresponding raw data representations, making it easy to work with predefined values in your code. The `nil` keyword, although not having a raw data representation, can still be used as any other piece of data in the instructions provided below.


# Instructions

The following table contains the various commands available in this programming language, along with their descriptions and return values:

| Common Commands | Description | Returns |
| --------------- | ----------- | ------- |
| `:=`            | Create or update a symbol in the immediate environment | Cell that was assigned |
| `set`           | Update an existing symbol in the current or parent environment | Cell that was assigned |
| `drop`          | Drop a symbol from the current or parent environment | `nil` |
| `try`           | Attempt to execute a list and handle any built-in exceptions | Last cell yielded from the executed list |
| `throw`         | Throw an exception that can be caught by `try` or will result in an interpreter halt | NA |
| `assert`        | Assert a given condition to be true or throw an error | `nil` |
| `len`           | Retrieve the length of a string or list. Members of a different type will be stringed and measured | Integer |
| `<-`            | Return the value of a cell, yielding whatever execution may be happening | Variable |
| `if`            | If statement | Variable |
| `loop`          | A loop | Variable |
| `clone`         | Clone a variable | Variable |
| `fn`            | Define a function | Variable |
| `import`        | Import files | 0 |
| `use`           | Use a module | 0 |
| `exit`          | Exit the program | N/A |
| `eval`          | Evaluate a given string | Variable |
| `quote`         | Quote a list into a string | Variable |
| `nop`           | Do nothing | `nil` |
| `macro`         | Define a macro | Variable |
| `dict`          | Create a dictionary | The new dictionary |
| `extern-call`   | Call a C-function from a shared library | Variable |
| `alias`         | Use a new symbol to refer to the data behind another | `nil` |
| `exchange` | Update the value of a cell and return the old value of the cell | variable
| `str-set-at` | Update a string by inserting a value at a given index (negative indexing permitted) | updated string
| `defer`         | Defer an instruction to be executed at the end of the current execution context | `nil`

| Type Commands | Description | Returns |
| ------------- | ----------- | ------- |
| `type`        | Retrieve a string detailing the type of a given item | String |
| `str`         | Convert an item to a string type | Converted value |
| `int`         | Convert an item to an integer type | Converted value |
| `char`        | Convert an item to a character type | Converted value |
| `i8`          | Convert an item to the integer type | Converted value |
| `i16`         | Convert an item to the integer type | Converted value |
| `i32`         | Convert an item to the integer type | Converted value |
| `i64`         | Convert an item to the integer type | Converted value |
| `u8`          | Convert an item to the integer type | Converted value |
| `u16`         | Convert an item to the integer type | Converted value |
| `u32`         | Convert an item to the integer type | Converted value |
| `u64`         | Convert an item to the integer type | Converted value |
| `float`       | Convert an item to a float type | Converted value |
| `f32`         | Convert an item to the float type | Converted value |
| `f64`         | Convert an item to the float type | Converted value |
| `split`       | Convert an item to a list comprised of the raw elements of the given variable | Converted value |

| Memory Commands | Description | Returns |
| --------------- | ----------- | ------- |
| `mem-new`       | Allocate some memory manually on the heap | Pointer cell |
| `mem-del`       | Delete some memory on the heap | Pointer cell |
| `mem-cpy`       | Copy a memory object | Pointer cell |
| `mem-load`      | Load a piece of memory as a cell type | Converted cell |
| `mem-is-set`    | Check if a cell pointer has its pointer set to a space in memory | T/F |

| List Commands | Description | Returns |
| ------------- | ----------- | ------- |
| `>\|`         | Push value to the front of a list | Modified list cell |
| `\|<`         | Push value to the back of a list | Modified list cell |
| `iter`        | Iterate over a list | Iterated list |
| `at`          | Retrieve an index into a list | Cell at the given index |
| `<\|>`        | Spawn a list of a given size with a given value | New list |
| `<<\|`        | Pop front | List given without the first element |
| `\|>>`        | Pop back | List given without the last element |

| Arithmetic | Description | Returns |
| ---------- | ----------- | ------- |
|

 `+`        | Add a series of cells | Resulting value |
| `-`        | Subtract a series of cells | Resulting value |
| `/`        | Divide a series of cells | Resulting value |
| `*`        | Multiply a series of cells | Resulting value |
| `%`        | Modulo a series of cells | Resulting value |
| `**`       | Raise a series of cells to a power | Resulting value |

| Comparison | Description | Returns |
| ---------- | ----------- | ------- |
| `eq`       | Check for equality | Returns numerical 1 or 0 representing true / false |
| `neq`      | Check for inequality | Returns numerical 1 or 0 representing true / false |
| `<`        | Less than | Returns numerical 1 or 0 representing true / false |
| `>`        | Greater than | Returns numerical 1 or 0 representing true / false |
| `<=`       | Less than or equal to | Returns numerical 1 or 0 representing true / false |
| `>=`       | Greater than or equal to | Returns numerical 1 or 0 representing true / false |
| `and`      | Logical and | Returns numerical 1 or 0 representing true / false |
| `or`       | Logical or | Returns numerical 1 or 0 representing true / false |
| `not`      | Logical not | Returns numerical 1 or 0 representing true / false |

| Bitwise | Description | Returns |
| -------- | ----------- | ------- |
| `bw-lsh` | Left shift | Integer |
| `bw-rsh` | Right shift | Integer |
| `bw-and` | Bitwise and | Integer |
| `bw-or`  | Bitwise or | Integer |
| `bw-xor` | Bitwise xor | Integer |
| `bw-not` | Bitwise not | Integer |

The table above provides an organized and concise overview of the available instructions in this programming language, along with their descriptions and return values.

# Execution contexts

Execution contexts dictate the length of time a defined variable exits. When the context is exited, all declared variables are removed.
Instructions that create contexts within the global context are as follows: `fn` `if` `try`, and `loop`. If a variable is defined in one
of these instructions, then the variable will be removed.

# Type Prompting

In certain commands, specific type information is required. This information is provided to the commands through "type tags," which represent the corresponding C types.

| Type Tag | C Type               |
| -------- | -------------------- |
| :u8      | unsigned 8-bit int   |
| :u16     | unsigned 16-bit int  |
| :u32     | unsigned 32-bit int  |
| :u64     | unsigned 64-bit int  |
| :i8      | signed 8-bit int    |
| :i16     | signed 16-bit int   |
| :i32     | signed 32-bit int   |
| :i64     | signed 64-bit int   |
| :f32     | 32-bit floating point |
| :f64     | 64-bit floating point |
| :str     | char* (string)       |
| :nil     | void                 |
| :int     | signed 64-bit int   |
| :float   | 64-bit floating point |
| :ptr     | void pointer         |

All types listed as type tags have both a C type and a corresponding Nibi type. Nibi types that have a direct C representation are known as "trivial types" and are mentioned throughout this document. The type tags play a crucial role in providing type information for commands that operate on specific data types.
# Notation

In this programming language, the following notation is used:

- **S**: A symbol (non-keyword).
- **[]**: A data list.
- **[*]**: A data list that will have its members executed.
- **()**: A processable list (includes accessor lists `{}`).
- **<>**: A meta list that encompasses accepted notation into a field.
- **..**: Indicates a continuation without specified end.
- **STR**: An explicit `string` value.
- **I**: An explicit `integer` value.
- **RD**: Any raw data member (`string`, `integer`, `double`).
- **NU**: A numerical value (`integer`, `double`).

Example:
```
( keyword S < () .. S > )
```
This example indicates that the `keyword` explicitly takes a `symbol` as the first argument, and then it can take either a `processable list` or a `symbol` as the second argument.

The meta list `<>` encompasses only one parameter into the given keyword. The provided notation serves as a guide for understanding the syntax and usage of various elements in the language.
# Instruction Details

## Keywords

### Alias

Keyword: `alias`

| arg 1         | arg 2                 |
| ------------- | --------------------- |
| item to alias | symbol to use as alias |

Note: Variables starting with `$` and `:` will not be allowed as they are reserved for error tokens and meta commands.

Example:
```
( alias < () S [] RD > < S > )
```

### Direct Environment Assignment

Keyword: `:=`

| arg 1          | arg 2 |
| -------------- | ----- |
| symbol to designate | symbol or list that will yield the value to set |

Note: Variables starting with `$` and `:` will not be allowed as they are reserved for error tokens and meta commands.

Example:
```
( := S < () S > )
```

### Indirect Environment Assignment

Keyword: `set`

| arg 1                                       | arg 2                            |
| ------------------------------------------- | -------------------------------- |
| symbol or list that will yield the cell to update | symbol or list that will yield the value to set |

Example:
```
( set < () S > < () S > )
```

### Drop Assigned Item

Keyword: `drop`

A variable number of arguments accepted into `drop`, with the minimum being 1.

All arguments must be expressed as a `symbol`.

Example:
```
( drop < S .. > .. )
```

### Retrieve Length

Keyword: `len`

| arg 1                          |
| ------------------------------ |
| Item to retrieve the length of |

Note: If the item is not a list, it will be converted to a string, and the length of the string value will be returned.

Example:
```
( len < RD S () [] > )
```

### Return Item

Keyword: `<-`

| arg 1  |
| ------ |
| Item to return |

Example:
```
( <- < RD S () [] > )
```

### If / Else

Keyword: `if`

| arg 1           | arg 2                          | arg 3 (optional)              |
| --------------- | ------------------------------ | ----------------------------- |
| Condition to check | Body to execute if true       | Body to execute if false      |

Example:
```
( if <() RD [*]> <() RD [*]> )
```

### Loop

Keyword: `loop`

| arg 1          | arg 2                    | arg 3           | arg 4                |
| -------------- | ------------------------ | --------------- | -------------------- |
| Precondition  | Condition to check        | Postcondition  | Body                   |

Precondition will be executed before entering the loop.
Postcondition will take place at the end of each loop.
Anything declared within the loop will be scoped to the loop, including anything done in the precondition.

Example:
```
( loop <()> <()> <()> <() [*]> )
```

### Clone

Keyword: `clone`

| arg 1        |
| ------------ |
| Item to clone |

Example:
```
( clone <RD [] () S> )
```

### Function

Keyword: `fn`

All cells passed to a function will be done so by reference so they can modify any given item.
To pass an explicit copy, use `clone` when calling the function on the parameters to clone.

Example 1:
```
( fn <S> <[]> <() [*]> )
```

Example 2 (Anonymous function):
```
( fn <[]> <() [*]> )
```

Variable Number of Arguments:

Using the `:args` tag as the only argument to a function, a list of arguments of any size may be given. 

Example:
```lisp
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

Example:
```
( import <S ...> )
```

All symbols in an import must be of string type and not presented in any type of list.

The system will utilize any imports from `-i` or `--include` to look for files marked to be included that are not immediately present. The search priority is as follows:

1) The launch directory
2) Include directories as they appeared in order from the include flag
3) The home directory under the directory `~/.nibi` if the directory exists

### Use

Keyword: `use`

Indicate that we want to use an installed or relative module.

Example:
```
( use <S ...> )
```

Items brought in via `use` will be constructed in their own environment that must be accessed with an accessor list unless that module specifies an export that ties a new symbol to the inner items. See docs/example_modules for more information on module construction.

### Try

Keyword: `try`

Expects 2 parameters. The first will be executed, and if an exception occurs, then the second list will be executed with a variable `$e` present in the environment while the recovery list is being processed.

Both parameters will execute top-level data lists `[]` as a list of processable lists and execute each member.

Example:
```
( try < () [*] S > < () [*] S > )
```

### Throw

Keyword: `throw`

| arg 1 |
| ----  |
| Cell to execute and throw a string value of |

Note: Whatever is returned from the execution of arg 1 will be forcefully converted to a string for the given exception. If the result cannot be turned into a string, it will throw a `cell_access_exception` instead.

Example:
```
( throw < [*] () S RD > )
```

### Assert

Keyword: `assert`

| arg 1              | arg 2                                          |
| ------------------ | ---------------------------------------------- |
| Condition to check | String value to throw in an exception if assertion fails <optional> |

Examples:
```
( assert < () S > STR )
( assert < () S > )
```

### Exit

Keyword: `exit`

| arg 1                  |
| ---------------------- |
| Number to set as exit code - must be an integer |

Example:
```
( exit < NU S () > )
```

### Eval

Keyword: `eval`

| arg 1      |
| ---------- |
| String to evaluate |

Example:
```
( eval < S STR > )
```

### Quote

Keyword: `quote`

| arg 1  |
| ------ |
| Item to quote |

Example:
```
( quote < S () [] {} RD > )
```

### Str

Keyword: `str`

| arg 1  |
| ------ |
| Item to convert to string |

Example:
```
( str < S () RD > )
```

### Int

Keyword: `int`

| arg 1  |
| ------ |
| Item to convert to an integer |

Example:
```
( int < S () RD > )
```

### Float

Keyword: `float`

| arg 1  |
| ------ |
| Item to convert to a float |

Example:
```
( float < S () RD > )
```

### Split

| arg 1           | arg 2                  |
| --------------- | ---------------------- |
| Item to split   | Length (only required if arg 1 is a list) |

Note: Split will not resolve symbols in a given list.

Note: A value of `0` passed as argument 2 when argument 1 is a list will result in a copy of the list being returned.

Example 1:
```
( split < S () RD > )
```

Example 2 (With list):
```
( split < S [] > < NU S () > )
```

### Type

Keyword: `type`

| arg 1  |
| ------ |
| Item to get the type of |

Returns a string detailing the type of what `arg 1` resolves to.

Possible return values and their corresponding information:

| Return          | Information                                           |
| --------------- | ----------------------------------------------------- |
| aberrant        | Aberrant cells are under-the-hood cells that shouldn't be able to be accessed |
| nil             | A nil cell                                            |
| u8              | unsigned 8 int                                       |
| u16             | unsigned 16 int                                      |
| u32             | unsigned 32 int                                      |
| u64             | unsigned 64 int                                      |
| i8              | signed 8 int                                         |
| i16             | signed 16 int                                        |
| i32             | signed 32 int                                        |
| i64             | signed 64 int                                        |
| f32             | 32-bit floating point                                |
| f64             | 64-bit floating point                                |
| ptr             | A pointer                                            |
| string          | A string cell                                        |
| char            | A character                                          | 
| list:data       | A data list                                          |
| list:access     | An access list                                       |
| list:instruction | An instruction list                                  |
| list            | Undetermined list type                               |
| function        | Function                                             |
| environment     | An environment of cells                              |
| symbol          | A symbol                                             |
| dict            | A dictionary                                         |
| unknown         | Unknown type                                         |

Note: The cells `aberrant`, `list`, `symbol`, and `unknown` will only show up if something goes terribly wrong, or you are working on extending the language with a module or working on Nibi itself.

Example:
```
( type < S () [] RD > ) 
```

### Nop

Keyword: `nop`

No arguments given. Does nothing.

Example:
```
(nop)
```

## List Instructions

### Push Front

Keyword: `>|`

```
Note: The `|` is meant to represent the boundary of a list, with the `>` showing 
      a direction that the data is being inserted. Notation assumes that the "front" of
      a list is the "left" side when reading.
```

| arg 1  | arg 2  |
| ------ | ------ |
| List to push to | Value to push |

Example:
```
( >| < () [] S RD > [] )
```

### Push Back

Keyword: `|<`

```
Note: The `|` is meant to represent the boundary of a list, with the `>` showing 
      a direction that the data is being inserted. Notation assumes that the "front" of
      a list is the "left" side when reading.
```

| arg 1  | arg 2  |
| ------ | ------ |
| List to push to | Value to push |

Example:
```
( |< < () [] S RD > [] )
```

### Spawn list

Keyword: `<|>`

| arg 1  | arg 2  |
| ------ | ------ |
| Value to default all elements to | Size of the list that must resolve to integer >0 |

Example:
```
( <|> < () S RD [] > < () I > )
```

### Pop front

| arg 1  |
| ------ |
| List to pop the front of |

Example:
```
( <<| < S [] > )
```

### Pop back

| arg 1  |
| ------ |
| List to pop the back of |

Example:
```
( |>> < S [] > )
```

### Iterate

Keyword: `iter`

| arg 1  | arg 2  | arg3 |
| ------ | ------ | ---- |
| List to iterate | Symbol name to map value to | Instruction(s) to execute per item |

The symbol used in argument two will shadow any existing variable of the same name and map to whatever the value is of the list being iterated. At the end of the instruction, the symbol will be removed.

Example:
```
( iter < [] S > S < () [*] > )
```

### At

Keyword: `at`

| arg 1  | arg 2  |
| ------ | ------ |
| List to index into | Index to access |

**Note:** Arg 2 is required to evaluate to an integer type.

Example:
```
( at < S [] > < () S I > )
```

----

## Arithmetic

A variable number of arguments are accepted, with the minimum being 2, except for `-`, which will subtract the given value from 0 by default.

Example:
```
( op < S () .. > .. )
```

**Note:** `*` and `+` work on strings if the first item given to them is a string.



## Comparisons

All comparison operators take exactly 2 arguments.

`eq` and `neq` will accept any cell type and attempt to check equality.
Whatever the simple type the lhs takes on, `eq` and `neq` will attempt to convert the rhs for checking. 

This means if the lhs operand is a string, it will attempt to stringify the rhs, and so on with integers and doubles. If the lhs is a more complex type like a list or a function, both sides will be stringified for comparison.

The remaining comparisons require that the arguments are numerical types (integer, float).

```
( op < S () > < S () > )
```

----

## Bitwise Instructions

### Left Shift

Keyword: `bw-lsh`

| arg 1  | arg 2  |
| ------ | ------ |
| Value to shift | Amount to shift |

Example:
```
( bw-lsh <() NU> <() NU> )
```

### Right Shift

Keyword: `bw-rsh`

| arg 1  | arg 2  |
| ------ | ------ |
| Value to shift | Amount to shift |

Example:
```
( bw-rsh <() NU> <() NU> )
```

### Bitwise AND

Keyword: `bw-and`

| arg 1  | arg 2  |
| ------ | ------ |
| Value to shift | Amount to shift |

Example:
```
( bw-and <() NU> <() NU> )
```

### Bitwise OR

Keyword: `bw-or`

| arg 1  | arg 2  |
| ------ | ------ |
| Value to shift | Amount to shift |

Example:
```
( bw-or <() NU> <() NU> )
```

### Bitwise XOR

Keyword: `bw-xor`

| arg 1  | arg 2  |
| ------ | ------ |
| Value to shift | Amount to shift |

Example:
```
( bw-xor <() NU> <() NU> )
```

### Bitwise NOT

Keyword: `bw-not`

| arg 1  |
| ------ |
| Value to not |

Example:
```
( bw-not <() NU> )
```

### Defer

| arg 1 .. n |
| Instruction to defer |

At the end of the execution context in which the statement is defined, all
instructions deferred will be executed in-order.

```
( defer < () .. > )
```

### Macro

Keyword: `macro`

| arg 1 | arg 2       | arg 3 .. n |
|------ |------------ |---         |
| name  | params list | macro body |

Example:

```lisp
# Create a macro that defines a `while` loop

(macro while [condition body]
    (loop (nop) %condition (nop) %body))

# Usable as :

(while (eq true true) (io::println "ayyy"))
```

The first argument of the macro is the name that can be utilized like a function call to invoke it.

The second argument must resolve to a data list `[]` and contain symbols that will be used to substitute into the macro.

Every substitution of the word must be prefixed with `%`. These substitutions are 1-for-1, so all expected symbols must fulfill syntax requirements for their position.

If a substitution symbol with the same name and form as a parameter is to be omitted, prefixing it with `\` will ensure that the symbol is ignored like so:

```lisp
(use "io")
(macro example [param] (io::println "Hey look its a: \%param"))
(example "nope")
```

Output:
```
Hey look its a %param
```

## Dictionary

Keyword: `dict`

The `dict` call takes a variable number of arguments depending on the intended action.

Creating a dict:

```lisp
# Variant 1 (empty)
(:= my_dict (dict))

# Variant 2 (empty)
(:= my_dict (dict []))

# Variant 3 (populated)
(:= my_dict (dict [
    ["a" 33]
    ["b" 44]
]))

# Variant 4 (populated)
(:= non_dict_list [
    ["a" 33]
    ["b" 44]
])

(:= my_dict (dict non_dict_list))
```

All dictionary keys MUST be literal strings, and the values can be anything. Each entry in a list that constructs a dictionary must be a data list of size 2 with a literal string in slot 0 and the value in slot 1.

Accessing a dict / creating entries:

```lisp
# Create a new entry / overwrite existing without checking
(my_dict :let "key" some_value) # Anything is permitted in value

(set something (x :get "key"))  # Retrieve an existing value (throws if not exists)

# Set something and ensure that it exists first
(set (x :get "key") "some new value")

# Delete a value
(x :del "key")

# Retrieve a list of keys
(:= the_keys (x :keys))

# Retrieve a list of values
(:= the_vals (x :vals))
```

As can be seen above, a created dictionary acts like a function for accessing and updating with a command-like parameter that starts with a `:`.

Dict commands:

| Command | Action         |
|---------|----------------|
| `:let`  | Create an item in the dict, will overwrite |
| `:get`  | Retrieve a reference to a key's value |
| `:del`  | Delete a key if it exists. Returns 0 if key didn't exist, 1 otherwise |
| `:keys` | Get a list of keys |
| `:vals` | Get a list of references to values |

*Note:* The order that keys are printed or exist is not required to fit any particular order. A list of values or keys retrieved may vary in order.

Printing a dict:

Since dict symbols are essentially functions, stringing them has to be done via a different method than `str`.

Calling a dict symbol with NO parameters like this `(my_dict)` will prompt it to return a fully formed, quoted string of the dict.

## External Calls

Keyword: `extern-call`

Call an external library:

```lisp
(extern-call <library name> <function name> <[parameter c-types]> <return c-type>) 
```

If `library name` is `nil`, the symbol will be searched for based on currently loaded symbols. Otherwise, it must be a path to a library file as a string.

`Function name` is the function name as a string.

`Parameters` is a data list of type tags (listed above).

`Return c-type` is the C-type that represents the return type of the function.

Example:
```lisp
(extern-cell nil "printf" [:str] :int "Hello, world!")
```

## Manual Memory Commands

- Keyword: `mem-new`
  - Allocates a set-sized amount of memory.
  - Returns a `ptr` cell that points to the newly allocated memory.
  - If the argument is given `nil` instead of an integer, then the pointer returned will contain no allocated memory.
  - **Note about ptr cells:**
    - Pointer cells don't control the lifetime of the allocated memory. When the pointer cell is deleted, the memory stays allocated. This could cause a memory leak. To prevent this, ensure you use `mem-del` for everything allocated with `mem-new`.
    - Pointer cells don't copy data when cloned to another pointer cell. Both cells will point to the same data in memory.

- Keyword: `mem-del`
  - Deletes the allocated memory. This function takes any number of arguments and attempts to free them all. The last item freed will be returned.

- Keyword: `mem-cpy`
  - Copies data in memory.
  - If the first item is a standard "trivial" cell (direct C-Type integration), it will be copied to the heap at a given destination pointer.
  - If the first item is a pointer cell, the data will be copied to the destination.
  - In the event that the destination value has already been allocated, that data will be automatically freed before the copy takes place.

- Keyword: `mem-load`
  - Attempts to load a value from memory as a regular, trivial cell.
  - Requires the first argument to be a cell `type tag` (mentioned above) that indicates how much space to load from memory and how to represent it as a cell.
  - The second parameter must be a ptr cell.

- Keyword: `mem-is-set`
  - Checks if a ptr cell has been set or if it's not pointing to anything.
  - Returns T/F.

# Modules

A module is organized as follows:

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

## `mod.nibi`

The `mod.nibi` file contains optional settings and configurations for the module. Here are the possible settings:

- Optional settings:
  - `version`: Module version (string).
  - `authors`: List of authors (strings).
  - `description`: Module description (string).
  - `licenses`: List of licenses (strings).

- Load source files directly into an environment:
  - `sources`: List of source file names to load into the module's environment.

- If a dynamic library needs to be loaded:
  - `dylib`: List of function names offered by the dynamic library. Note: Dynamic libraries must take the name `<module_name>.lib`.

- Optional list of files to execute in-order immediately following an import:
  - `post`: List of file names to execute after importing. These can be used to redefine access to the module or run sanity checks before continuing.

## Module Tests

Tests for installed modules can be run via:

```
nibi -t <module_name>
```

This command will iterate over all detected test files and execute them to ensure that the module is working correctly.

## Notes on Modules

- When a module is loaded, it is populated into its own environment cell.
- Variables with a prefixed `_` will be private to the module, and accessing the module externally will be done via an accessor list `{}`.
- All files within a module are gathered into the same environment, so there is no need to `import` files internally.
- The environment will be populated in the order listed in `mod.nibi`, so top-level variables will only be aware of things loaded before them.
- If a function is executed at the top-most level and attempts to access a yet-to-exist cell, a runtime error will occur.

## Module Installation

Modules can be located anywhere within the nibi include directories, but a module is not considered installed until it exists in `NIBI_PATH/modules`.

# Applications

Applications are any directories with a `main.nibi` file within them. Applications can have a `tests` directory that is automatically discovered and executed, similarly to modules. Applications are launched at the directory level as follows:

```
nibi ~/path/to/app
```
