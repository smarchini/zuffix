#!/usr/bin/env python3
import random
import string
import gmpy2
from time import sleep

#-------------------------------------------------------------------------------
# Functions
#-------------------------------------------------------------------------------
def randstr(n, dollar=False):
    s = ''.join(random.choices(list('abc'), k=n))
    # Use '~'=chr(127) instead of '$'=chr(36)
    return s[:-1] + '~' if dollar else s

def nu(j):
    assert j >= 0, "nu is undefined"
    return bin(j)[2:].count('1')

def rho(j):
    assert j != 0, "rho is inf"
    return gmpy2.bit_scan1(j)

def lbda(j):
    assert j > 0, "lambda is undefined"
    return len(bin(j)[2:]) - 1

def twoFattest(x, y):
    allone = (1 << 64) - 1
    return y & (allone << lbda(x ^ y))

def twoFattestL(x, y):
    return twoFattest(x+1, y-1)
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Suffix array construction algorithms
#-------------------------------------------------------------------------------
def saca(T):
    return sorted(range(len(T)), key=lambda i: T[i:])

def lcpca(T, SA):
    return [-1] + [ lcp(T[SA[i]:], T[SA[i+1]:]) for i in range(len(SA)-1) ] + [-1]

def lcp(a, b):
    i = 0
    while i < len(a) and i < len(b) and a[i] == b[i]: i += 1
    return i

def udnca(LCP):
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

def ctca3(LCP, UP, DOWN, NEXT):
    n = len(LCP) - 1
    CLD = [-10000] * n
    CLD[0] = NEXT[0]
    for i in range(1, n):
        if LCP[i-1] > LCP[i]: CLD[i-1] = UP[i]
        if LCP[DOWN[i]] > LCP[i]: CLD[i] = DOWN[i]
        if LCP[NEXT[i]] == LCP[i]: CLD[i] = NEXT[i]
    CLD[n-1] = UP[n]
    return CLD

def ctca(LCP):
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
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Suffix array traversal algorithms
#-------------------------------------------------------------------------------
def getlcp3(i, j, LCP, UP, DOWN):
    if i < UP[j] < j:
        return LCP[UP[j]]
    return LCP[DOWN[i]]

def getlcp(i, j, LCP, CT):
    if i < CT[j-1] < j:
        return LCP[CT[j-1]]
    return LCP[CT[i]]

def getChild3(T, SA, LCP, UP, DOWN, NEXT, i, j, a):
    d = getlcp3(i, j, LCP, UP, DOWN)
    l = i
    if i < UP[j] < j:
        r = UP[j]
    else:
        r = DOWN[i]
    assert d == LCP[r]
    while r != 0 and (SA[l] + d >= len(T) or T[SA[l] + d] != a):
        l = r
        if NEXT[r] == 0:
            return (l, j)
        r = NEXT[r]
    if SA[l] + d < len(T) and T[SA[l] + d] == a: return (l, r)
    return (0, -1)

def getChild(T, SA, LCP, CT, i, j, a):
    l = i
    if i < CT[j-1] < j:
        r = CT[j-1]
    else:
        r = CT[i]
    d = LCP[r]
    while l < r and (SA[l] + d >= len(T) or T[SA[l] + d] != a):
        l = r
        if LCP[r] != LCP[CT[r]] or LCP[r] > LCP[r+1]:
            return (l, j)
        r = CT[r]
    if SA[l] + d < len(T) and T[SA[l] + d] != a: return (0, -1)
    return (l, r)
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Simple search algorithms
#-------------------------------------------------------------------------------
def binarySearchSA(T, P, SA, l, r):
    if r <= l: return (l, r)
    c = (l + r) // 2
    i = 0
    while i < len(P):
        if P[i] < T[SA[c] + i]: return binarySearchSA(T, P, SA, l, c)
        if P[i] > T[SA[c] + i]: return binarySearchSA(T, P, SA, c+1, r)
        i += 1
    L = binarySearchSA(T, P, SA, l, c)
    R = binarySearchSA(T, P, SA, c+1, r)
    return (min(c, L[0]), max(c, R[1]))

def acceleratedBinarySearchSA(T, P, SA, l, r, llcp, rlcp):
    if r <= l: return (l, r)
    c = (l + r) // 2
    i = min(llcp, rlcp)
    while i < len(P) and SA[c] + i < len(T):
        if P[i] < T[SA[c] + i]: return acceleratedBinarySearchSA(T, P, SA, l, c, llcp, i)
        if P[i] > T[SA[c] + i]: return acceleratedBinarySearchSA(T, P, SA, c+1, r, i, rlcp)
        i += 1
    L = acceleratedBinarySearchSA(T, P, SA, l, c, llcp, i)
    R = acceleratedBinarySearchSA(T, P, SA, c+1, r, i, rlcp)
    return (min(c, L[0]), max(c, R[1]))
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Enhanced search algorithms
#-------------------------------------------------------------------------------
def enhancedStringSearch(T, P, SA, LCP, CT):
    c = 0
    i, j = 0, len(T)
    while i < j and c < len(P):
        i, j = getChild(T, SA, LCP, CT, i, j, P[c])
        if j - i == 1:
            if SA[i] + len(P) > len(T): return (1, 0)
            elif T[SA[i] + c : SA[i] + len(P)] != P[c : len(P)]: return (1, 0)
            break
        d = getlcp(i, j, LCP, CT)
        d = min(d, len(P))
        if T[SA[i] + c : SA[i] + d] != P[c : d]: return (1, 0)
        c = d
    return (i, j)

def enhancedStringSearch3(T, P, SA, LCP, UP, DOWN, NEXT):
    c = 0
    i, j = 0, len(T)
    while i < j and c < len(P):
        i, j = getChild3(T, SA, LCP, UP, DOWN, NEXT, i, j, P[c])
        if j - i == 1: # it is a leaf
            if SA[i] + len(P) > len(T): return (1, 0)
            elif T[SA[i] + c : SA[i] + len(P)] != P[c : len(P)]: return (1, 0)
            break
        d = getlcp3(i, j, LCP, UP, DOWN)
        d = min(d, len(P))
        if T[SA[i] + c : SA[i] + d] != P[c : d]: return (1, 0)
        c = d
    return (i, j)
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# TODO Zuffification
#-------------------------------------------------------------------------------
#def zfill(Z, T, LCP, CT, i, j, name):
#    if i <= j + 1: return  # leaves are not in Z
#    # l, r = getChild2(T, SA, LCP, CT, i, j, a): TODO integrarla
#    handle = twoFattest(name - 1, extent)
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Test: ttatctctta
#-------------------------------------------------------------------------------
T = 'ttatctctta'
SA = saca(T)
LCP = lcpca(T, SA)
UP, DOWN, NEXT = udnca(LCP)
CT = ctca(LCP)
assert CT == ctca3(LCP, UP, DOWN, NEXT)
# print me if you want
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Test: abracadabra~
#-------------------------------------------------------------------------------
T = 'abracadabra~'
SA = saca(T)
LCP = lcpca(T, SA)
UP, DOWN, NEXT = udnca(LCP)
CT = ctca(LCP)
assert CT == ctca3(LCP, UP, DOWN, NEXT)
# print me if you want
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Test: random strings
#-------------------------------------------------------------------------------
def assertMatch(T, P, SA, l, r):
    for i in range(l, r): # [l, r)
        assert T[SA[i] : SA[i] + len(P)] == P

def testRandom(n):
    cnt = 0
    for i in range(1, n):
        T = randstr(n, dollar=True)
        P = randstr(random.randint(1, min(10, len(T))))
        SA = saca(T)
        LCP = lcpca(T, SA)
        UP, DOWN, NEXT = udnca(LCP)
        CT = ctca(LCP)
        assert CT == ctca3(LCP, UP, DOWN, NEXT)
        (l1, r1) = binarySearchSA(T, P, SA, 0, len(T)-1)
        (l2, r2) = acceleratedBinarySearchSA(T, P, SA, 0, len(T)-1, 0, 0)
        (l3, r3) = enhancedStringSearch(T, P, SA, LCP, CT)
        (l4, r4) = enhancedStringSearch3(T, P, SA, LCP, UP, DOWN, NEXT)
        if l1 >= r1 or l2 >= r2 or l3 >= r3:
            assert l1 >= r1 and l2 >= r2 and l3 >= r3 and l4 >= r4, f'Expected: {l1} >= {r1} and {l2} >= {r2} and {l3} >= {r3} and {l4} >= {r4}'
        else:
            assert l1 == l2 == l3 == l4 and r1 == r2 == r3 == r4, f'Expected: {l1} == {r1} and {l2} == {r2} and {l3} == {r3} and {l4} == {r4}'
            assertMatch(T, P, SA, l1, r1)
            cnt += 1
    return cnt

for _ in range(100): testRandom(10)
for _ in range(100): testRandom(100)
for _ in range(100): testRandom(127)
#-------------------------------------------------------------------------------
