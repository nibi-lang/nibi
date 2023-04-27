## Ignore anything here



```

(:= my_list [1 2 3 4 5])

# push front and return pointer to list
(>| 3 my_list)

# push back and return pointer to list
(|< 2 my_list)

# pop front and return pointer to list
(<| my_list)

# pop back and return pointer to list
(|> my_list)

# iterate over my_list exceute whatever is in the second argument
# for every element that is in the list
# $it will be available during the course of the execution that
# will represent the current item in the list with $idx being 
# the numerical index that that value exists at
(iter my_list [
  $it

  $idx

])


# retrieve length of the list
(len my_list)

# reverse the list and return pointer to list
(rev my_list)

# retrieve element at index of list
(at my_list 0)

```