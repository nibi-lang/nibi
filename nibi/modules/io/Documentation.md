# Module: IO

A basic I/O module to extend Nibi

| method  | exported as    | parameters | returns
|----     |----            |----        |----
| get_str | io::get_str    |  NONE      | Resulting string
| get_int | io::get_int    |  NONE      | Integer if input from user valid, NIL otherwise
| get_int | io::get_double |  NONE      | Integer if input from user valid, NIL otherwise
| prompt_for | io::prompt  | prompt, function | Return value of given function parameter
