
(env global)

; tag groups define types - :symbol :integer :float :expr :string :vec :any :ref

; Define a tag group

(tag-group vor (resolves-as :ref))  ; resolves-as permits symbols iff they are known to resolve-as vector at compile time

(routine let (target-name :symbol value :any)

  ; Parent indicates to use the active environment from caller
  (put @parent target-name (eval value))
)

(routine set (target-name :symbol value :any)
  (get @parent target-name)
  (if (eval x0)
      (throw "Target with given name " target-name " already exists"))
  (put @parent target-name (eval value))

:int
    :unsigned-int
        :u8 :u16 :u32 :u64
    :signed-int
        :i8 :i16 :i32 :i64

:real
    :f32 :f64

:num
    :int :real

:signed
    :signed-int
    :real

All types can be suffixed with '[]' to indicate a vector as-in:   :num[]

User-defined objects create a tag for themselves once created. Must be unique

(def-obj str
  (data :u8[])
  (fn :str new () (<- self))
  (fn :str new (val :u8[]) (
    (set self.data (clone val))
    (<- self))))


Traits can be defined as such:

(def-trait iterator ($self $type-a)     ; Indicate that traits must be arguments
    (fn :u32 enumerate(self :self))
    (fn :type-a get_n(self :self n :u32)))

(impl-trait iterator (:str :u8)
    (fn :u32 enumerate(self :str) (len self.data))
    (fn :u8 get_n(self :str n :u32) (at self.data n)))

; Which allows:

(str name "Josh")

(iter x name 
  (put x " "))      ; "J o s h "   

; These might be able to be macros...
;
;           ------  -   -   -   -   -   --------
;


(trait iterator ($self $type-a)
    (fn :u32 enumerate (self :self t :type-a) #list.. )
    (fn :type-a get_n(self :self n :u32) #list.. ))

(obj str 
  (data :u8)
  (impl (iterator (:str :u8)) 
     (fn :u32 enumerate(self :str) (len self.data))
     (fn :u8 get_n(self :str n :u32) (at self.data n))
  )
)

(def-macro assert (lhs :any rhs :any)
  (if (not (eq (eval lhs) (eval rhs))) (throw "Assertion faileure")))

(def-macro assert-true (x :any)
  (assert! true x))

(def-macro impls (var :symbol t :trait)
    (is-subset (members t) (members (type-of var))))

 (def-macro insist (val :list constraint :trait)
    (assert-true! (impls! (at val 0) constraint)))

 (def-macro iter (as :symbol target (insist! (target :symbol) 'iterator) body #list..)
    ; Define iter here
    

:type           declare item is of type
:type[]         declare item a vector of type
:type{}         declare item a set of type

#type           expect an instance of a type in a macro
.. #type        repeated types until end of list inside a macro (.. is saying that all remaining items should be of the next tag)
#type[]         expect an instance of a type in a macro
.. #type[]      repeated types until end of list inside a macro
$type-name      accept a type tag as a parameter in a list ()






base language... before macros

declarative types

:int
    :unsigned-int
        :u8 :u16 :u32 :u64
    :signed-int
        :i8 :i16 :i32 :i64

:real
    :f32 :f64

:num
    :int :real

:signed
    :signed-int
    :real

:type[] -> vector of type

:list -> indicate that the thing isn't to be processed (raw list)




(let x :list (putln "Hey")) -> Creates a list 

(eval x)        -> executes the list


let
set
at      - vector method on object
len     - vector method on object
+
-
/
*
fn

(let x :u8[] (vec 1 2 3 4 5 6))     ; Implicit vec calling can be done later

(putln (x.at 0))


(let x :any[] (vec a b c 3 4.3 "moot")) ; any should be a special object later made like:

(obj any
  (value-type :type-id)     ; Some identifier that can be matched against known types
  (value :neb)              ; Re-think the name, but this stands for nebulous
)

(let g :any 4)
(match g.type
    (:u8 (putln "Its a u8"))
    (:u32 (putln "Its a u32")))


- All code must be contained in a top level declaration 
    (module, macro, fn, etc)


(fn main :i8 ()         ; Until str object and types/traits are made, we can take no input
    ())



