# Nibi

[![CircleCI](https://dl.circleci.com/status-badge/img/gh/bosley/nibi/tree/main.svg?style=svg&circle-token=209b10db36b5ca69404370b19addef558e676ecf)](https://dl.circleci.com/status-badge/redirect/gh/bosley/nibi/tree/main)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

## About

The Nibi programming language! Nibi is a list processing language that is under heavy development.

For documentation on the language please see `docs/LANGUAGE.md` 

[![asciicast](https://asciinema.org/a/594596.svg)](https://asciinema.org/a/594596)

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

`Required Libs`:

```
libffi-dev
```

To get started, clone the repo and ensure you run:

```
    git submodule update --init --recursive
```

Before installing `nibi` you will need to create the `NIBI_HOME` environment variable that directs
to an area that modules and everything can be installed to. 

Once that is done use `commander.py` to help build/ install Nibi and its default modules.

You can use `-h` to see the options, or just use `-n -m -c -t` 
to build nibi, build/ install the modules, run module checks, and automated tests to ensure
that the system is setup correctly.

# Applications

A Nibi application is any directory with a `main.nibi` located within it. From here, any `tests` directory will be associated with the application and any non-installed module described here
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

