#!/usr/bin/env python3
import random
import string
from time import sleep

# Use '~'=chr(127) instead of '$'=chr(36)
def randstr(n, dollar=False):
    s = ''.join(random.choices(list('abc'), k=n))
    return s[:-1] + '~' if dollar else s

def lcp(a, b):
    i = 0
    while i < len(a) and i < len(b) and a[i] == b[i]:
        i += 1
    return i

def saca(T):
    return sorted(range(len(T)), key=lambda i: T[i:])

def lcpca(T, SA):
    return [-1] + [ lcp(T[SA[i]:], T[SA[i+1]:]) for i in range(len(SA)-1) ] + [-1]

def ctca(LCP):
    n = len(LCP) - 1
    up, down, nextl = [0]*(n+1), [0]*(n+1), [0]*(n+1)
    stack = [0]
    lastIndex = 0
    for i in range(1, n+1):
        while LCP[i] < LCP[stack[-1]]:
            lastIndex = stack.pop()
            if LCP[i] <= LCP[stack[-1]] and LCP[stack[-1]] != LCP[lastIndex]:
                down[stack[-1]] = lastIndex
        if LCP[i] == LCP[stack[-1]]:
            nextl[stack[-1]] = i
        if lastIndex != 0:
            up[i] = lastIndex
            lastIndex = 0
        stack.append(i)
    stack.pop()
    assert stack == [0]
    return (up, down, nextl)

def cldca(LCP, UP, DOWN, NEXT):
    n = len(LCP) - 1
    CLD = [-10000] * n
    CLD[0] = NEXT[0]
    for i in range(1, n):
        if LCP[i-1] > LCP[i]: CLD[i-1] = UP[i]
        if LCP[DOWN[i]] > LCP[i]: CLD[i] = DOWN[i]
        if LCP[NEXT[i]] == LCP[i]: CLD[i] = NEXT[i]
    CLD[n-1] = UP[n]
    return CLD

def cldca2(LCP):
    n = len(LCP) - 1
    CT = [0] * n
    stack = [0]
    lastIndex = 0
    for i in range(1, n+1):
        while LCP[i] < LCP[stack[-1]]:
            lastIndex = stack.pop()
            if LCP[i] <= LCP[stack[-1]] and LCP[stack[-1]] != LCP[lastIndex]:
                CT[stack[-1]] = lastIndex
        if LCP[i] == LCP[stack[-1]]:
            CT[stack[-1]] = i
        if lastIndex != 0:
            CT[i-1] = lastIndex
            lastIndex = 0
        stack.append(i)
    stack.pop()
    assert stack == [0]
    return CT

def getlcp(i, j, LCP, UP, DOWN):
    if i < UP[j] < j:
        return LCP[UP[j]]
    return LCP[DOWN[i]]

def getlcp2(i, j, LCP, CT):
    if i < CT[j-1] < j:
        return LCP[CT[j-1]]
    return LCP[CT[i]]

#print(f'l = {l}, r = {r}, T[{SA[l]}] = {T[SA[l]]}, T[{SA[r]}] = {T[SA[r]]}, NEXT[{r}] = {NEXT[r]}')
def getChild(T, SA, LCP, UP, DOWN, NEXT, i, j, a): # d = profondità (in caratteri) del livello
    d = getlcp(i, j, LCP, UP, DOWN)
    l = i
    if i < UP[j] < j:
        r = UP[j]
    else:
        r = DOWN[i]
    assert d == LCP[r]
    while r != 0 and (SA[l] + d >= len(T) or T[SA[l] + d] != a):
        #if SA[l] + d < len(T) and T[SA[l] + d] == a: break
        l = r
        r = NEXT[r]
    if r == 0:
        r = j
    if SA[l] + d < len(T) and T[SA[l] + d] == a:
        return (l, r)
    return (0, -1)

def getChild2(T, SA, LCP, CT, i, j, a):
    l = i
    if i < CT[j-1] < j:
        r = CT[j-1]
    else:
        r = CT[i]
    d = LCP[r]
    while l < r and (SA[l] + d >= len(T) or T[SA[l] + d] != a):
        l = r
        print(f'l = {l}, r = {r}, d = {d}, c = {a}, lcp[r] = {LCP[r]}, lcp[ct[r]] = {LCP[CT[r]]}')
        if LCP[r] != LCP[CT[r]]:
            return (l, j)
        r = CT[r]
        print(f'l = {l}, r = {r}, sa[l] + d = {SA[l] + d}, t[sa[l] + d] = {T[SA[l] + d]}, c = {a}' )
    return (l, r)

def isEmpty(i, j):
    return j <= i

def enhancedStringSearch(T, P, LCP, UP, DOWN, NEXT):
    found = True
    c = 0
    i, j = getChild(T, SA, LCP, UP, DOWN, NEXT, 0, len(T), P[0])
    print(f'getChild(T, SA, LCP, UP, DOWN, NEXT, {i}, {j}, "{P[c]}")')
    while not isEmpty(i, j) and c < len(P) and found:
        if j - i > 1:
            d = getlcp(i, j, LCP, UP, DOWN)
            print(f'd = {d}')
            m = min(d, len(P))
            found = T[SA[i] + c : SA[i] + m] == P[c : m] # occhio a - 1
            c = m
            if c < len(P):
                print(f'c = {c}, getChild(T, SA, LCP, UP, DOWN, NEXT, {i}, {j}, "{P[c]}")')
                i, j = getChild(T, SA, LCP, UP, DOWN, NEXT, i, j, P[c])
        else:
            found = T[SA[i] + c : SA[i] + len(P)] == P[c : ]# occhio a - 1
            print(f'else: found = {found} = T[{SA[i]} + {c} : {SA[i]} + {len(P)}] == P[{c} : ]')
            break
    if found and not isEmpty(i, j):
        return (i, j)
    else:
        return "non trovato"


def enhancedStringSearch(T, P, LCP, UP, DOWN, NEXT):
    found = True
    c, i, j = 0, 0, len(T)
    while c < len(P) and not isEmpty(i, j):
        i, j = getChild(T, SA, LCP, UP, DOWN, NEXT, i, j, P[c])
        d = getlcp(i, j, LCP, UP, DOWN)
        print(f'child = [{i}, {j}), d = {d}')
        if d == -1: d = len(T)
        m = min(d, len(P))
        found = T[SA[i] + c : SA[i] + m] == P[c : m] # occhio a - 1
        c = m
    if found and not isEmpty(i, j):
        return (i, j)
    else:
        return "non trovato"

def enhancedStringSearch2(T, P, LCP, CT):
    found = True
    c, i, j = 0, 0, len(T)
    while c < len(P) and not isEmpty(i, j):
        i, j = getChild2(T, SA, LCP, CT, i, j, P[c])
        d = getlcp2(i, j, LCP, CT)
        print(f'd = {d}')
        if d <= 0: d = len(T)
        m = min(d, len(P))
        print(f'i = {i}, j = {j}, d = {d}, m = {m}, c = {c}')
        found = T[SA[i] + c : SA[i] + m] == P[c : m] # occhio a - 1
        c = m
    if found and not isEmpty(i, j):
        return (i, j)
    else:
        return "non trovato"



T = 'ttatctctta'
SA = saca(T)
LCP = lcpca(T, SA)
UP, DOWN, NEXT = ctca(LCP)
CT = cldca2(LCP)
assert CT == cldca(LCP, UP, DOWN, NEXT)
enhancedStringSearch(T, "ta", LCP, UP, DOWN, NEXT)
enhancedStringSearch2(T, "ta", LCP, CT)
enhancedStringSearch(T, "t", LCP, UP, DOWN, NEXT)
enhancedStringSearch2(T, "t", LCP, CT)
enhancedStringSearch(T, "tac", LCP, UP, DOWN, NEXT)
enhancedStringSearch2(T, "tac", LCP, CT)


T = 'abracadabra~'
SA = saca(T)
LCP = lcpca(T, SA)
UP, DOWN, NEXT = ctca(LCP)
CT = cldca2(LCP)
assert CT == cldca(LCP, UP, DOWN, NEXT)
enhancedStringSearch(T, "ab", LCP, UP, DOWN, NEXT)
enhancedStringSearch2(T, "ab", LCP, CT)
enhancedStringSearch(T, "t", LCP, UP, DOWN, NEXT)
enhancedStringSearch2(T, "t", LCP, CT)
enhancedStringSearch(T, "aba", LCP, UP, DOWN, NEXT)
enhancedStringSearch2(T, "aba", LCP, CT)



def binarySearchSA(T, P, SA, l, r):
    if r < l: return (l, r)
    c = (l + r) // 2
    i = 0
    while i < len(P):
        if P[i] < T[SA[c] + i]: return binarySearchSA(T, P, SA, l, c-1)
        if P[i] > T[SA[c] + i]: return binarySearchSA(T, P, SA, c+1, r)
        i += 1
    L = binarySearchSA(T, P, SA, l, c-1)
    R = binarySearchSA(T, P, SA, c+1, r)
    return (min(c, L[0]), max(c, R[1]))

def acceleratedBinarySearchSA(T, P, SA, l, r, llcp, rlcp):
    if r < l: return (l, r)
    c = (l + r) // 2
    i = min(llcp, rlcp)
    while i < len(P) and SA[c] + i < len(T):
        if P[i] < T[SA[c] + i]: return acceleratedBinarySearchSA(T, P, SA, l, c-1, llcp, i)
        if P[i] > T[SA[c] + i]: return acceleratedBinarySearchSA(T, P, SA, c+1, r, i, rlcp)
        i += 1
    L = acceleratedBinarySearchSA(T, P, SA, l, c-1, llcp, i)
    R = acceleratedBinarySearchSA(T, P, SA, c+1, r, i, rlcp)
    return (min(c, L[0]), max(c, R[1]))

def testSome(n):
    cnt = 0
    for i in range(1, n):
        T = randstr(n, dollar=True)
        P = randstr(random.randint(1, 10))
        SA = saca(T)
        x = binarySearchSA(T, P, SA, 0, len(T) - 1)
        y = acceleratedBinarySearchSA(T, P, SA, 0, len(T) - 1, 0, 0)
        if x != y:
            print(f"Error: {x} != {y}")
            return
        elif x[0] <= x[1]:
            print(f'T = {T}, P = {P}, count = {x[1] - x[0] + 1}')
            cnt += 1
    return cnt

T = 'abracadabra~'
P = 'a'
SA = saca(T)
print(SA)
print(binarySearchSA(T, P, SA, 0, len(T)-1))
print(acceleratedBinarySearchSA(T, P, SA, 0, len(T)-1, 0, 0))

for p in [ "0", "z", "bracad", "abracad", "abracadabra" ]:
    print(p, "=>", acceleratedBinarySearchSA(T, p, SA, 0, len(T)-1, 0, 0))
