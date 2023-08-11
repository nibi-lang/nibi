# Nibi

[![CircleCI](https://dl.circleci.com/status-badge/img/gh/nibi-lang/nibi/tree/main.svg?style=svg)](https://dl.circleci.com/status-badge/redirect/gh/nibi-lang/nibi/tree/main)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

## About

The Nibi programming language! Nibi is a list processing language that is under heavy development.

For documentation on the language please see `docs/LANGUAGE.md` 

## Demo applications

### Fizzbuzz

```lisp
(use "io")
(loop (:= x 1) (< x 20) (set x (+ 1 x)) [
  (if (and (not (% x 3)) (not (% x 5)))
      (io::println "FizzBuzz")
        (if (not (% x 3)) 
          (io::println "Fizz")
          (if (not (% x 5)) 
            (io::println "Buzz") 
            (io::println x))))])
```

### Leibniz pi estimation

```lisp
(fn leibniz [n] [
  (:= sum 0.0)
  (:= term 0.0)
  (loop (:= i 0.0) (< i n) (set i (+ i 1.0)) [
    (set term (/ (** -1.0 i) (+ 1.0 (* 2.0 i 1.0))))
    (set sum (+ sum term))
  ])
  (<- (* sum 4))
])
(leibniz 65536)
```

### Magic number game

```lisp
(use "io")
(use "random")

(:= magic_number (random::range::int 0 100))

(io::println "We've generated a random number between 0 and 100!")
(io::println "Go ahead and guess until you get it right!")

(:= num_guesses 1)
(loop (:= i 0) (eq 0 i) (set num_guesses (+ 1 num_guesses)) [
   (:= guess (io::prompt "guess: " io::get::int))
   (if (eq magic_number guess) [
      (io::println "Correct!")
      (io::println "You got the number correct in " num_guesses " guesses!")
      (exit 0)
   ])
   (if (< magic_number guess) (io::println "Lower!") (io::println "Higher!"))
])
```

## Installing

### Linux 

(Only tested on Ubuntu and Pop!OS)

`Required Libs`:

```
libffi-dev
```

Before installing `nibi` you will need to create the `NIBI_PATH` environment variable that directs
to an area that modules and everything can be installed to. 

Once that is done use `commander.py` to help build/ install Nibi and its default modules.

You can use `-h` to see the options, or just use `-n -m` 
to build nibi, build/ install the modules.

Once nibi is built and installed, `ldconfig` may need to be ran so the application can locate the library `libnibi` that
was just installed.

Verify that the nibi application was installed by running `which nibi`. This should display something similar to
`/usr/local/bin/nibi`. If nothing shows up there was an error in installing. Run commander again and see if any
errors are prompted. 

Once the intallation is confirmed, check the system to ensure nibi and modules are running correctly with `commander.py -c -t`.
This will run checks on all installed nibi modules, and then run their respective test suites.

### Apple

(Only tested on Ventura with M2 silicon)

To build and install nibi on apple hardware, the following steps should be taken:

- Install gcc and pkconfig (with brew or ports)
- Make sure the following or equivalents are set:
```
export PKG_CONFIG_PATH="/usr/local/opt/libffi/lib/pkgconfig"
export CXX="/opt/homebrew/bin/g++-13"
export CC="/opt/homebrew/bin/gcc-13"
alias gcc="/opt/homebrew/bin/gcc-13"
alias g++="/opt/homebrew/bin/g++-13"
```

( Apple REALLY likes clang so forcing gcc is a bit of a trip )

Note about MacOS:

The network and fileio module will not work correctly and as-such have been stubbed 
to throw errors when attempting to use them.

Since this variant of nibi is POC only, it is unlikely they will be updated to support apple, though
doing so wouldn't be too much work

# REPL

Of course nibi has a repl! Just type in `nibi` once its installed and you will be prompted to start using the language.

# Input

Nibi can be setup to be executed by placing a shebang line at the top of the file that pointes to the application on disk:
`#!/usr/local/bin/nibi` and `chmod`ing the file with the `-x` option. 

If this is done, then nibi scripts can then take in data piped to it like so:

```

cat some_file.txt | cool_parser.nibi

```

Of course nibi applications/ scripts also take in input arguments as one might suspect:

```
./cool_parser.nibi some_file.txt


nibi cool_parser.nibi some_file.txt
```

# Applications

A Nibi application is any directory with a `main.nibi` located within it. From here, any `tests` directory will be associated with the application (described below) and any non-installed module (described below) located here
can be utilized within the application. Applications can be ran at the directory level as such:

```
nibi /path/to/app
```

An example application can be seen in `docs/example_app`

# Modules

Modules are extensions of the language and can be created in C++ or in Nibi itself.. or both! Examples of how to build a module can be seen in `docs/example_modules`. 
A module is considered `installed` when it exists along with its (optional) compiled `.lib` in `$NIBI_PATH/modules`.

# Testing

Nibi offers the ability to run tests in a given module or application via the `-t` parameter. Any module or application path passed to Nibi with the given `-t` flag will have its top-most directories scanned for a `tests` directory. Every file within the test directory will be executed to ensure that the module or application is correct as-per the test specifications. 

`Note:` An application with submodules will not automatically have its local and non-installed sub-modules' tests run when the app itself is passed to be tested. Only the application-specific tests will be executed at that time. 

