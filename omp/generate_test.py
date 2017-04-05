import random
import itertools
import string

def product_no_repeats(*args):
    for p in itertools.product(*args):
        if len(set(p)) == len(p):
            yield p

def reservoir(it, k):
    ls = [next(it) for _ in range(k)]
    for i, x in enumerate(it, k + 1):
        j = random.randint(0, i)
        if j < k:
            ls[j] = x
    return ls

cell_number = 5

xs = range(0, cell_number)
ys = range(0, cell_number)
zs = range(0, cell_number)

file = open("ex.in","w")

unique_cell_list = reservoir(product_no_repeats(xs, ys, zs), cell_number)
s = "%d\n" % cell_number
file.write(s)
for i in range(0,cell_number):
	file.write("%d %d %d\n" % (unique_cell_list[i][0], unique_cell_list[i][1], unique_cell_list[i][2]))

file.close()

