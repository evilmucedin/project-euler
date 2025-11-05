#!/usr/bin/env python3


import inflect

n = 2+2
print(n)

p = inflect.engine()
print(p.number_to_words(n))
