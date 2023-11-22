
Basic types and tags

```
:u8 :u16 :u32 :u64
:i8 :i16 :i32 :i64
:f32 :f64

:list   - Indicating an unprocessed list
          Arguments passed must take the form `()` and
          they will not be executed before passing.
          Arguments passed as such must be `eval`d
          to execute.
```

-----------------------------
 
| Prefix   | Meaning         |
|----      |----             |
|   :      | standard tag    |
|   ^      | reference tag   | 
|   $      | macro tag param |

| Suffix   | Meaning         |
|----      |----             |
|   +      | Vector of type  |

Tags prefixed with `^` indicate that the given tag name is a reference
to the object.

Example:

^u8 ^u16 .. ^f64

Unprocessed lists with `list` tag prefixed with `^` should
act the same as a `:list` - where the execution env is the same
no matter where its executed > Perhaps we will use this to execute
the list in the caller's env.

Tags prefixed with `$` are reserved for macro definitions.
In macros we want to have the caller be able to indicate the type
the macro should expect, so `$` is essentially just a way to pass
along types.

Tags suffixed with `+` indicate that the passed type
will be a `vector` of the given type.

Notice that `:str` is not mentioned. Strings are not
a primitive type. All strings will be recognized as a 
vector of unsigned integers. Once the basic type system
and object definitions are created, strings of various
sizes will be defined, but until then a `:u8+` can be 
utilized.

The front-end parser will take common `""` string 
definitions and generate a `+u8+` vector.

-----------------------------

User-defined objects will have a tag generated for the given
name, placed into the same scope as the object definition.

-----------------------------

Initial keywords

let
set
fn
len
at
<-

-----------------------------

Unlike other s-expression languages there will not be a repl at first.
All code must be contained within functions at first, with an
entry method defined.

As the language develops we can implement a repl in the language
and add other top-level items such as `module` `import`, and
global constants.




-----------------------------
        SCRATCH PAD
-----------------------------
```

(fn main :i8 (args :u8+)

  (putln args)      ; Until higher objects defined, multiple strings
                    ; will have to be handled by receiving functions.
                    ; The parser will `\0` all strings parsed, so
                    ; determining string boundaries in most cases
                    ; will be just fine. Eventually `args` will
                    ; be replaced with :u8str+ that will define a 
                    ; structure for each string as sub-vectors
 (<- 0)
)


```
-----------------------------
