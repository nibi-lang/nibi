
# Instruction

| keyword | description | returns
|----     |----         |----
| :=      | Create or update a symbol in the immediate environment | cell that was assigned
| set     | Update an existing symbol in current or parent environment  | cell that was assigned
| drop    | Drop a symbol from current or parent environment | nil
| try     | Attempt to execute a list and handle any built in exceptions | last cell yielded from executed list
| throw   | Throw an exception that can be caught by `try` or will result in a interpreter halt | na
| assert  | Assert a given condition to be true or throw an error | nil
| len     | Retrieve the length of a string or list. Members of a different type will be stringed and measured | integer
| <-      | Return the value of a cell, yielding whatever execution may be happening | variable
| ?       | If statement | variable
| loop    | A loop | variable
| clone   | clone a variable | variable
| put     | Output a string representation of N cells | 0
| putln   | put, but with a following newline         | 0
| fn      | Define a function | variable
| env     | Define an environment | new env
| import  | Import files | 0

| @ commands | description | returns
|----       |----          |----
| @debug    | Toggle debug on/off | nil 

| debug commands | description | returns
|----       |----          |----
| dbg     | Execute cells iff debug enabled | nil 
| dbg-out | Print given cells iff debug enabled | nil
| dbg-var | Construct a string detailing information about given cells | string

| list commands | description | returns
|----   |---- |----
| >\|    | Push value to front of list | modified list cell
| \|<    | Push value to back of list  | modified list cell
| iter  | Iterate over a list         | iterated list
| at    | Retrieve an index into a list | cell at given index
| <\|>  | Spawn a list of a given size with a given value | new list

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

### Direct environment assignment

Keyword: `:=`

| arg1 | arg2 |
|----  |----
| symbol to designate | symbol or list that will yield the value to set

```
  Note: Variables starting with `$` will not be allowed. 
```

```
( := S < () S > )
```

### Indirect environment assignment

Keyword: `set`

| arg1 | arg2 |
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

| arg1
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

| arg1
|----
| Item to return

```
( <- < RD S () [] > )
```

### If / Else

Keyword: `?`

| arg1               | arg2                    | arg3 (optional)
|----                |----                     |----
| Condition to check | Body to execute if true | Body to execute if false

```
( ? <() RD [*]> <() RD [*]> )
```

### Loop

Keyword: `?`

| arg1          | arg2               | arg3           | arg4
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

| arg1
|----
| Item to clone

```
( clone <RD [] () S> )
```

### Put

Keyword: `put`

Variable number of arguments supported, at least 1 required

```
( put <RD [] () S> ... )
```

### Putln

Keyword: `putln`

Variable number of arguments supported, `0+`

```
( putln <RD [] () S> ... )
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

### Environment

Keyword: `env`

Create an environment with functions and data members
that can be isolated from other information.


```
( env <S> <() [*]>)
```

Accessing an environment from outside is done so with an accessor list `{}`
that details all environments and sub envrionments up-to the desired cell
is located.

All environmnet members with a leading `_` are considered private and can not
be accessed from any other enviornment than the one they are defined in.

Example: 

```
(env dummy_env [
  (:= public_data 0)
  (:= _private_data 0)
  (fn public_accessor_private_data [] (<- _private_data))
  (fn public_setter_private_data [value] (set _private_data value))
])

# Accessing dummy_env's public data:

(assert (eq {dummy_env public_data} 0) "Could not access public item")

# Setting the public data

(set {dummy_env public_data} 1024)

(assert (eq {dummy_env public_data} 1024) "Could not access public item")

(assert (eq ({dummy_env public_accessor_private_data}) 0) "Could not retrieve private data from public accessor")

({dummy_env public_setter_private_data} 2048)

(assert (eq ({dummy_env public_accessor_private_data}) 2048) "Private data not updated")

```

### Environment

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

| arg1 |
|----  |
| cell to execute and throw string value of

Note: Whatever is returned from the execution of arg1 will be forcefully converted to a string for the given
exception. If the result can not be turned into a string it will throw a cell_access_exception instead.

This means that if you want to directly update the cell that `$it` points to, you NEED to use `set`
and not `:=`. The former will find where it points and update it, the latter will overwrite $it into env
and be overwritten at the end of the instruction list.

```
( throw < [*] () S RD > )
```

### Assert

Keyword: `assert`

| arg1 | arg2 |
|----  |----
| condition to check | string value to throw in an exception if assertion fails

```
( assert < () S > STR )
```

-----

## Debug Instructions


### Toggle Debug

Keyword: `@debug`

| arg1 |
|----  |
| integer value 1 or 0 |

```
( @debug < 1 0 > )
```

### Debug Printing

Keyword: `dbg-out`

A variable number of arguments accepted with the minimum being 1

```
( dbg-out < [] () S .. > .. )
```

### Debug Variable

Keyword: `dbg-var`

A variable number of arguments accepted with the minimum being 1

```
( dbg-var < S .. > .. )
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

| arg1 | arg2 |
|----  |----
| value to push | list to push to

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

| arg1 | arg2 |
|----  |----
| value to push | list to push to

```
( |< < () [] S RD > [] )
```

### Spawn list

keyword: `<|>`

| arg1 | arg2 |
|----  |----
| value to default all elements to| size of the list that must resolve to integer >0

```
( <|> < () S RD [] > < () I > )
```

### Iterate

keyword: `iter`

| arg1            | arg2                        | arg3 |
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

| arg1               | arg2 |
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

A variable number of arguments accepted with the minimum being 2

```
( op < S () .. > .. )
```

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

| arg1 | arg2 |
|----  |----
| value to shift | amount to shift

```
( bw-lsh <() NU> <() NU> )
```

### Right Shift

keyword: `bw-rsh`

| arg1 | arg2 |
|----  |----
| value to shift | amount to shift

```
( bw-rsh <() NU> <() NU> )
```
### Bitwise AND

keyword: `bw-and`

| arg1 | arg2 |
|----  |----
| value to shift | amount to shift

```
( bw-and <() NU> <() NU> )
```
### Bitwise OR

keyword: `bw-or`

| arg1 | arg2 |
|----  |----
| value to shift | amount to shift

```
( bw-or <() NU> <() NU> )
```

### Bitwise XOR

keyword: `bw-xor`

| arg1 | arg2 |
|----  |----
| value to shift | amount to shift

```
( bw-xor <() NU> <() NU> )
```
### Bitwise NOT

keyword: `bw-not`

| arg1
|----
| value to not

```
( bw-not <() NU> )
```