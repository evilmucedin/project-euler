def btwinner_(pieces):
	for i in range(len(pieces)):
		if pieces[i] >= 2:
			for j in range(pieces[i] - 2 + 1):
				cp = pieces[:]
				cp[i] = j
				cp.append(pieces[i] - 2 - j)
				if not btwinner(cp):
					return True
	return False

import pickle

btcache = {}
def btwinner(pieces):
	pieces = filter(lambda x: x != 0, pieces)
	pieces = sorted(pieces)
	key = pickle.dumps(pieces)
	if not key in btcache:
		btcache[key] = btwinner_(pieces)
	return btcache[key]

cacheGrundy = []

cacheGrundy.append(0)
cacheGrundy.append(0)

def grundy_(n):
	used = set()
	for i in range(n - 1):
		used.add( grundy(i) ^ grundy(n - i - 2) )
	i = 0
	while i in used:
		i += 1
	return i

def grundy(n):
	while n >= len(cacheGrundy):
		cacheGrundy.append( grundy_(len(cacheGrundy)) )
	return cacheGrundy[n]

count = 0
for i in range(1, 340):
	if grundy(i) != 0:
		count += 1
count34 = 0
for i in range(500, 500 + 34):
	if grundy(i) != 0:
		count34 += 1

N = 1000000

border = (N - (N % 34))

count += (border - 340)//34*count34

for i in range(border, N + 1):
	if grundy(340 + (i % 34)) != 0:
		count += 1

print(count)