# Nibi

[![CircleCI](https://dl.circleci.com/status-badge/img/gh/bosley/nibi/tree/main.svg?style=svg&circle-token=209b10db36b5ca69404370b19addef558e676ecf)](https://dl.circleci.com/status-badge/redirect/gh/bosley/nibi/tree/main)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

## About

The Nibi programming language! Nibi is a list processing language that is under heavy development.

To get started, you can run `commander.py` to help build/ install Nibi and its default modules.

For documentation on the language please see `docs/LANGUAGE.md` 

## Demo applications

### Fizzbuzz

```lisp
(loop (:= x 1) (< x 20) (set x (+ 1 x)) [
  (? (and (not (% x 3)) (not (% x 5)))
      (putln "FizzBuzz")
        (? (not (% x 3)) 
          (putln "Fizz")
          (? (not (% x 5)) 
            (putln "Buzz") 
            (putln x))))])
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

(putln "We've generated a random number between 0 and 100!")
(putln "Go ahead and guess until you get it right!")

(:= num_guesses 1)
(loop (:= i 0) (eq 0 i) (set num_guesses (+ 1 num_guesses)) [
   (:= guess (io::prompt "guess: " io::get::int))
   (? (eq magic_number guess) [
      (putln "Correct!")
      (putln "You got the number correct in " num_guesses " guesses!")
      (exit 0)
   ])
   (? (< magic_number guess) (putln "Lower!") (putln "Higher!"))
])
```

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

