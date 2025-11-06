#!/usr/bin/env python3


import inflect

a = int(input("a: "))
b = int(input("b: "))
n = a + b
print(n)

p = inflect.engine()
print(p.number_to_words(n))
