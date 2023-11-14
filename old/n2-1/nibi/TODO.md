

- Debugger

When debug flag is set and file/dir given, we should pop into an interactive shell
that provides the ability to set break points in a given target file(s), run,
and restart the app, etc. A would-be-nice future expansion would be the ability
to edit the file in-place and a tui... perhaps generating a whole other
application on build would be a good idea for this. (ndb ?)

It would be very beneficial to show bytecode next to the nibi code and have it
update in real time as its being edited and ran. 


Would want to modify app/ to be apps/ and have 
    apps/nibi
    apps/ndb
    apps/... etc
