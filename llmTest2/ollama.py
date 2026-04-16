#!/usr/bin/env python

import ollama

response = ollama.generate(model='llama3', prompt='Write a poem about Linux.')

with open('poem.txt', 'w') as f:
    f.write(response['response'])

