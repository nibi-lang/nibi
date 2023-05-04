
# Instruction

| keyword | description | returns
|----     |----         |----
| :=      | Create or update a symbol in the immediate environment | cell that was assigned
| set     | Update an existing symbol in current or parent environment  | cell that was assigned
| drop    | Drop a symbol from current or parent environment | nil
| try     | Attempt to execute a list and handle any built in exceptions | last cell yielded from executed list
| throw   | Throw an exception that can be caught by `try` or will result in a runtime halt | na
| assert  | Assert a given condition to be true or throw an error | nil
| len     | Retrieve the length of a string or list. Members of a different type will be stringed and measured | integer
| <-      | Return the value of a cell, yielding whatever execution may be happening | variable

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

# Notation

**S** - A symbol (non keyword)

**[]** - Data list

**[*]** - Data list that will have its members executed

**()** - Processable list

**<>** - A meta list to plug all accepted notation into a field

**..** - Indicating a continuation without specifed end

**STR** - An explicit `string` value

**I** - An explicit `integer` value

**RD** - Any raw data member (`string, integer, double`) 

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
