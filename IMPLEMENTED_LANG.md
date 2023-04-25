
List types

Lists defined with `()` are processable lists meaning that the first element is 
expected to be a function that can be executes, with the remaining data being parameters
into that function.

Lists defined with `[]` are considered data lists meaning the first element isn't 
expected to be an executable, however, a data list of processable items may be 
handled as a series of commands to be executed depending on the command that its
used with (see: try)


Implemented Keywords:

```

@debug

:=
drop

+
-
/
*
%
**

dbg
dbg-out
dbg-var

try

```

# AT Commands

The at commands are built in functions starting with `@` that execute specific changes
to the language runtime functionality

### @debug

Takes explicitly 1 input arg (2 total, with parameter forced to be an integer)
Sets the runtime debug flag to the boolean value of the evaluated parameter

# Debug

### dbg

Execute whatever is given to the `dbg` command if and only if debug is enabled. 
Executes all top level data lists as processable lists.

```
(dbg () () ... [() ... ()] ...)
```

### dbg-var

Debug takes any number of params, and will generate a string representing all parameters
given and their information. (does not print string)

NOTE: This instruction will return NIL if the runtime has `debug` disabled

### dbg-out

Dumps whatever is given to string with a new line.

NOTE: This instruction will NOT be executed if the runtime has `debug` disabled

Returns true iff the message was output

# Non-Debug Instructions

### try

Expects 3 parameters. The first will be executed and if an exception occurs
then the second list will be executed with a variable `$e` present in the 
environment while the recovery list is being processed.

Both parameters will execute top level data lists `[]` as a list of processable
lists, and execute each member.

```

<try> <attempt_list> <recovery_list>

(try () ())

(try [() ... ] ())

(try () [() ... ])

(try [() ... ] [() ...])
```

Example:

```

(:= item 0)
(drop item)

(try (:= x item) [
  (dbg-out "Looks like there was an exception:")
  (dbg-out $e)
])

```
