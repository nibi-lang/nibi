# "compiling"

We _could_ do a "bytecode" (encoded instructions) that dump to some file. The purpose would be to
slice into the _builtins_ map that the runtime executes and instead of having functions that execute the code
it could just run over all of the checks (making sure length is correct, types correct, etc) and then when
we run the actual instruction set later we can bypass the checks to go faster

This could be done without rewriting `runtime` by simply telling the `list_builder` to retrieve
a different `builtins` map that the runtime will pipe calls to that do the checks and emit the bytecode somewhere. 

