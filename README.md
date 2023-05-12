# nibi

The nibi programming language! Nibi is a list processing language that is under heavy development.

To get started, you can run `commander.py` to help build/ install Nibi and its modules.

A demo fizz buzz:

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