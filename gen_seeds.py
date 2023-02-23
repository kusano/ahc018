import random

random.seed(1337)

for W in range(1, 5):
  for K in range(1, 11):
    for C in [1, 2, 4, 8, 16, 32, 64, 128]:
      for i in range(10):
        print(random.randint(0, 2**64-1), W, K, C)
