


Implemented Keywords:

```

+
-
/
*
%
**
:=
drop


```

# Debug

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

(try (:= x item) (dbg-out $e))

```
