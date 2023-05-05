# "compiling"

We _could_ do a "bytecode" (encoded instructions) that dump to some file. The purpose would be to
slice into the _builtins_ map that the interpreter executes and instead of having functions that execute the code
it could just run over all of the checks (making sure length is correct, types correct, etc) and then when
we run the actual instruction set later we can bypass the checks to go faster

This could be done without rewriting `interpreter` by simply telling the `list_builder` to retrieve
a different `builtins` map that the interpreter will pipe calls to that do the checks and emit the bytecode somewhere. 



- Register VS stack based to consider. 

Registers could be cells that have members updated rather than being constructed all the time, and the cell could
act as an interface to the underlying data

