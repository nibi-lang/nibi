# Module: sys

A systems module

| method  | exported as    | parameters | returns
|----     |----            |----        |----
| get_argv | sys::args | none | list of arguments given to nibi
| get_platform | sys::platform | none | string detailing current operating system
| get_stdin | sys::stdin | none | list of data piped into nibi
| get_nibi_path | sys::homw | none | String of nibi path if exists


| functions | parameters | returns
|----       |----        |----
| sys::linux| none       | true iff system is linux
| sys::macos| none       | true iff system is macos
| sys::windows | none    | true iff system is windows
| sys::nix  | none       | true if linux or mac
| sys::known_os | none   | true iff system type deteceted
