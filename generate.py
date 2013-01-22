from random import randint, choice, random
from math import exp

def operator():
  print choice("+-*/"),

def expression(level = 0):
  number = random() > (0.9 * exp(-level/2))
  if number:
    print randint(0, 1000),
  else:
    if level == 0: parens = choice([True] + [False]*5)
    else: parens = True
    if parens: print "(",

    expression(level + 1)
    for i in xrange(randint(0, 3)):
      operator()
      expression(level + 1)

    if parens: print ")",

expression()
print ";"
