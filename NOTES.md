

# Debug Mode

Debug command won't get executed in `dbg` mode isn't flagged via CLI or enabled in the file...
since lists are processed as they are read in, we can use an input command like:

(@debug 1)

that could flag the list builder to drop the list and not even send it to the runtime


(@runtime (set_debug 1)) ?? 




