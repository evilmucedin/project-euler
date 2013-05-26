import math

cacheGrundy = []

cacheGrundy.append(0)

def grundy_(n):
	top = int(math.sqrt(n))
	used = set()
	for i in range(1, top + 1):
		used.add( grundy(n - i*i) )
	i = 0
	while i in used:
		i += 1
	return i

def grundy(n):
	while n >= len(cacheGrundy):
		cacheGrundy.append( grundy_(len(cacheGrundy)) )
	return cacheGrundy[n]

N = 100000

grundy(N + 1)

print("grundy is ready")

counts = []
for i in range(6):
	smI = [0]
	prev = 0
	for j in range(N):
		if grundy(j) == i:
			prev += 1
		smI.append(prev)
	counts.append(smI)

print("counts are ready")

count = 0
for i in range(N):
	print(i)
	for j in range(i, N):
		xor = grundy(i) ^ grundy(j)
		if xor < 6:
			count += counts[xor][N] - counts[xor][j]

print(count)
