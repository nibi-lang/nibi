## (ﾉ◕ヮ◕)ﾉ*✲ﾟ*｡⋆ You've made a PR!

Before the code PR is merged, please ensure that the following checklist is completed. 

- [ ] Branch name details the work done for the PR
- [ ] CI tests have passed
- [ ] An admin has reviewed and accepted the PR
- [ ] Clang format has been run on changes (`run ./tools/format.sh`)

Please be sure that the code follows the style as follows:

- Everything in `snake_case`
- Classes end in `_c` (unless its an interface, then end in `_if`)
- Structs end in `_s`
- Unions end in `_u`
- Enumerations end in `_e`
- Type names that are redefined via `using` end in `_t` (always use `using` - not `typedef`)
- Type names that define a particular type of pointer end in `_ptr`

The above is just a loose formatting that has been used already throughout the C++ source code of Nibi,
and we would like to keep things mostly uniform. In the future, an actual style guide may be created. 

## Description of work:

Please enter a description of your work here:
```

```
