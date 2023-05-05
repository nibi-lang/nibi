# nibi

A demo fizz buzz 

```lisp
(@debug 1)
(loop (:= x 1) (< x 50) (set x (+ 1 x)) [
  (? (and (not (% x 3)) (not (% x 5)))
      (dbg-out "FizzBuzz")
        (? (not (% x 3)) 
          (dbg-out "Fizz")
          (? (not (% x 5)) 
            (dbg-out "Buzz"))))])
```