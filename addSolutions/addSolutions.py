#!/usr/bin/env python3


import inflect

p = inflect.engine()

while True:
    a = int(input("a: "))
    b = int(input("b: "))
    n = a + b
    print(n)

    print(p.number_to_words(n))
