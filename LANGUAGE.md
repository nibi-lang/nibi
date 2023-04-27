
# Instruction

| keyword | description | returns
|----     |----         |----
| :=      | Create or update a symbol in the immediate environment | cell that was assigned
| set     | Update an existing symbol in current or parent environment  | cell that was assigned
| drop    | Drop a symbol from current or parent environment | nil
| try     | Attempt to execute a list and catch/ handle exceptions | last cell yielded from executed list
| assert  | Assert a given condition to be true or throw an error | nil

| @ commands | description | returns
|----       |----          |----
| @debug    | Toggle debug on/off | nil 

| debug commands | description | returns
|----       |----          |----
| dbg     | Execute cells iff debug enabled | nil 
| dbg-out | Print given cells iff debug enabled | nil
| dbg-var | Construct a string detailing information about given cells | string

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

**STR** - An explicit `string` value expected

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

### Assert

| arg1 | arg2 |
|----  |----
| condition to check | string value to throw in an exception if assertion fails

```
( assert < () S > STR )
```

-----

## Debug Instructions


### Toggle Debug

| arg1 |
|----  |
| integer value 1 or 0 |

```
( @debug < 1 0 > )
```

### Debug Printing

A variable number of arguments accepted with the minimum being 1

```
( dbg-out < [] () S .. > .. )
```

### Debug Variable

A variable number of arguments accepted with the minimum being 1

```
( dbg-var < S .. > .. )
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

`eq` and `neq` will accept any cell type and attempt to check equality - non numerical items will be converted to a string for comparisons

the remaining comparisons require that the arguments are numerical types (integer, float)

```
( op < S () > < S () > )
```
