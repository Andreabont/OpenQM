from itertools import product

n = 5
for item in product(*[['0', '1']] * (2 ** 5)):
    print(''.join(item))
