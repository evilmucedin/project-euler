import heapq

maxN = 10000000
primes = [True]*maxN
primes[1] = False
lPrimes = []

heap = []

for i in range(2, maxN):
	if primes[i]:
		lPrimes.append(i)
		
		now = i
		while now < maxN:
			heapq.heappush(heap, now)
			now = now*now

		j = i + i
		while j < maxN:
			primes[j] = False
			j += i

print(len(heapq))
M = 500500507
ans = 1
for h in range(500500):
	now = heapq.heappop(heap)
	ans = (ans*now) % M
print(ans)