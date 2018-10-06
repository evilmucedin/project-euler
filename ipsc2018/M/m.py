#!/usr/bin/env python3
import requests
import math
from json import dumps, loads
from time import sleep

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

while True:
    r = q({"action": "refresh"})
    # print(r.text)
    resp = loads(r.text)
    if 'active_customers' in resp:
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

    sleep(1)
