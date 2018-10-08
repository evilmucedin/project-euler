#!/usr/bin/env python3
import requests
import math
from json import dumps, loads
from time import sleep
import sys

def q(data):
    return requests.post('https://ipsc.ksp.sk/2018/real/problems/m_api', cookies={"ipsc2018ann": "5", "ipscsessid": "0e067cda98ebdbb84b1f8b304d8fedc181098ba9"}, data=data)

def submit(customerId, answer):
    data = {"action": "submit", "customer_id": customerId, "answer": answer}
    print(data)
    q(data)

def fromBin(x):
    return int(x, 2)

def toBin(x):
    return bin(x)[2:]

def factorial(x):
    res = 1
    for i in range(1, x + 1):
        res *= i
    return res

def intToRoman(input):
   if type(input) != type(1):
      raise TypeError("expected integer, got %s" % type(input))
   if not 0 < input < 4000:
      raise ValueError("Argument must be between 1 and 3999")
   ints = (1000, 900,  500, 400, 100,  90, 50,  40, 10,  9,   5,  4,   1)
   nums = ('M',  'CM', 'D', 'CD','C', 'XC','L','XL','X','IX','V','IV','I')
   result = ""
   for i in range(len(ints)):
      count = int(input / ints[i])
      result += nums[i] * count
      input -= ints[i] * count
   return result

def romanToInt(input):
   if type(input) != type(""):
      raise TypeError("expected string, got %s" % type(input))
   input = input.upper()
   nums = ['M', 'D', 'C', 'L', 'X', 'V', 'I']
   ints = [1000, 500, 100, 50,  10,  5,   1]
   places = []
   for c in input:
      if not c in nums:
         raise ValueError("input is not a valid roman numeral: %s" % input)
   for i in range(len(input)):
      c = input[i]
      value = ints[nums.index(c)]
      # If the next place holds a larger number, this value is negative.
      try:
         nextvalue = ints[nums.index(input[i +1])]
         if nextvalue > value:
            value *= -1
      except IndexError:
         # there is no next place.
         pass
      places.append(value)
   sum = 0
   for n in places: sum += n
   # Easiest test for validity...
   return sum


assert(toBin(1) == "1")
assert(fromBin("1") == 1)
assert(toBin(fromBin("1")) == "1")
assert(toBin(fromBin("0")) == "0")
assert(toBin(fromBin("101010010101")) == "101010010101")
assert(toBin(fromBin("11111")) == "11111")
assert(romanToInt("XV") == 15)
assert(intToRoman(romanToInt("XV")) == "XV")

def isNested(s):
    stack = []
    for ch in s:
        if ch == '(' or ch == '[':
            stack.append(ch)
        elif ch == ')':
            if 0 == len(stack):
                return False
            if stack[-1] == '(':
                stack.pop()
            else:
                return False
        elif ch == ']':
            if 0 == len(stack):
                return False
            if stack[-1] == '[':
                stack.pop()
            else:
                return False
        else:
             raise Exception('Bad char "%s"' % s)
    return len(stack) == 0

assert(isNested("([])"))
assert(not isNested("()("))
assert(not isNested("([)]"))

def ticTac(s):
    s = s.replace("\\n", "\n")
    parts = s.split("\n")
    assert(len(parts) == 3)
    for i in range(3):
        assert(len(parts[i]) == 3)
    for ch in ['X', 'O']:
        for i in range(3):
            if parts[i][0] == ch and parts[i][1] == ch and parts[i][2] == ch:
                return ch
            if parts[0][i] == ch and parts[1][i] == ch and parts[2][i] == ch:
                return ch
        if parts[0][0] == ch and parts[1][1] == ch and parts[2][2] == ch:
            return ch
        if parts[0][2] == ch and parts[1][1] == ch and parts[2][0] == ch:
            return ch
    return "NEITHER"

assert(ticTac("OXO\nXXO\nX.O") == 'O')
assert(ticTac("OXO\nXXO\nX..") == 'NEITHER')
assert(ticTac("OXO\\nXXO\\nX..") == 'NEITHER')
assert(ticTac("X.O\nOOX\n..X") == 'NEITHER')

def rotate(s1, s2):
    s1 = s1.replace("\\n", "\n")
    parts1 = s1.split("\n")
    s2 = s2.replace("\\n", "\n")
    parts2 = s2.split("\n")
    parts1 = [list(x) for x in parts1]
    parts2 = [list(x) for x in parts2]

    d1 = list(sorted([len(parts1), len(parts1[0])]))
    d2 = list(sorted([len(parts2), len(parts2[0])]))

    def rot(x):
        return list(list(x) for x in list(zip(*x[::-1])))

    if d1 != d2:
        return False

    r = parts2[:]
    for i in range(4):
        if parts1 == r:
            return True
        # print(r, rot(r))
        r = rot(r)

    for i in range(len(parts2)):
        parts2[i] = parts2[i][::-1]

    r = parts2[:]
    for i in range(4):
        if parts1 == r:
            return True
        r = rot(r)

    return False

assert(rotate("@..\n@@@\n@..\n@..", '@@@@\n..@.\n..@.'))
assert(rotate('@@.\n.@@\n.@@', ".@@\n.@@\n@@."))
assert(rotate('@@.\n.@@\n.@@', '@@.\n.@@\n.@@'))
assert(rotate('@@..\n.@@@\n.@..', '..@@\n@@@.\n..@.'))
assert(rotate('@@@@\n@..@', '@@\n@.\n@.\n@@'))
assert(not rotate('.@@@@\n@@...', '@@@@@\n..@..'))

def islands(s):
    s = s.replace("\\n", "\n")
    parts = s.split("\n")
    for i in range(len(parts)):
        parts[i] = list(parts[i])
    count = 0
    for i in range(len(parts)):
        for j in range(len(parts[i])):
            if parts[i][j] == '#':
                count += 1

                def erase(i, j):
                    if i >= 0 and i < len(parts):
                        if j >= 0 and j < len(parts[i]):
                            if parts[i][j] == '#':
                                parts[i][j] = '.'
                                erase(i - 1, j)
                                erase(i + 1, j)
                                erase(i, j - 1)
                                erase(i, j + 1)

                erase(i, j)
    return count

assert(islands("#.##.\n#....\n..###\n....#\n##...") == 4)
assert(islands('..#.#\n....#\n..#..\n#.#.#\n#.#..') == 5)

# sys.exit(0)

first = True

while True:
    r = q({"action": "refresh"})
    if r.status_code != 200:
        continue
    resp = loads(r.text)
    if 'active_customers' in resp:
        if first:
            print("Serving")
            first = False
        ac = resp['active_customers']
        if len(ac) > 0:
            customer1 = ac[0]
            print(len(ac))
        for customer1 in ac:
            print(customer1)
            if 'text' in customer1:
                text = customer1['text']
                if 'customer_id' in customer1 and 'var1' in customer1:
                    customerId = customer1['customer_id']
                    var1 = customer1['var1']
                    if text == "HELLO I WOULD LIKE TO ORDER FOOD ITEM var1.":
                        submit(customerId, var1)
                    if text == "OLLEH, I MA A BOTOR TAHT SKLAT SDRAWKCAB. EVIG EM DOOF var1 ESAELP.":
                        submit(customerId, var1[::-1])
                    if text == "GOOD DAY COUGH GIVE ME FOOD var1 COUGH COUGH COUGH. PARDON ME, THAT WAS A BUFFER OVERFLOW. I WANTED TO SAY THE FIRST 110 DIGITS OF THAT, PLEASE IGNORE THE REST.":
                        submit(customerId, var1[0:6])
                    if text == "GIVE ME ANY FOOD BETWEEN 1 AND 1000, INCLUSIVE. BUT NOT var1, I HATE THAT.":
                        if var1 == "1":
                            submit(customerId, "10")
                        else:
                            submit(customerId, "1")
                    if text == "I WANT THE SQUARE ROOT OF var1.":
                        submit(customerId, toBin(int(math.sqrt(fromBin(var1) + 1e-9))))
                    if text == "I AM SO EXCITED! GIVE ME var1 AND YES, OF COURSE THAT IS A FACTORIAL!":
                        var = var1[0:(len(var1) - 1)]
                        submit(customerId, toBin(factorial(fromBin(var))))
                    if text == "'I HEARD var1 IS VERY TASTY, BUT I DO NOT UNDERSTAND THAT FOREIGN LANGUAGE. WHAT IS IT CALLED IN BINARY?":
                        submit(customerId, toBin(eval(var)))
                    if text == "Ave, popina dominus! Lorem ipsum! I heard you have Roman recipes too. Give me food var1 + var2.":
                        var2 = customer1['var2']
                        submit(customerId, intToRoman(romanToInt(var1) + romanToInt(var2)))
                    if text == "I HEARD var1 IS VERY TASTY, BUT I DO NOT UNDERSTAND THAT FOREIGN LANGUAGE. WHAT IS IT CALLED IN BINARY?":
                        submit(customerId, toBin(eval(var1)))
                    if text == "I JUST WANT TO KNOW: IS var1 CORRECTLY NESTED? PLEASE ANSWER YES OR NO.":
                        submit(customerId, "YES" if isNested(var1) else "NO")
                    if text == "A STRANGE GAME. THE ONLY WINNING MOVE IS NOT TO PLAY.\nBUT WHO WON? ANSWER X OR O OR NEITHER.\n\nvar1":
                        submit(customerId, ticTac(var1))
                    if text == "HOW MANY ISLANDS ARE ON THIS MAP?\n\nvar1":
                        submit(customerId, toBin(islands(var1)))
                    if text == "CAN YOU CHANGE THIS SHAPE TO THAT SHAPE JUST BY ROTATING OR FLIPPING IT? ANSWER YES OR NO.\n\nvar1\n\n\n\nvar2":
                        var2 = customer1['var2']
                        submit(customerId, "YES" if rotate(var1, var2) else "NO")


    sleep(1)
