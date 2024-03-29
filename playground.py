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
    # come da descrizione, quindi (x .. y]
    allone = (1 << 64) - 1
    return y & (allone << lbda(x ^ y))

def twoFattestL(x, y):
    return twoFattest(x+1, y-1)

def twoFattestLR(x, y):
    if x == 0: return 0
    allone = (1 << 64) - 1
    return y & (allone << lbda((x-1) ^ y))
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
            # return (l, j)  # WARNING NOPE, getChild3 might be wrong too!
            r = j
            break
        r = CT[r]
    if SA[l] + d < len(T) and T[SA[l] + d] != a: return (0, -1)
    return (l, r)
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Simple search algorithms
#-------------------------------------------------------------------------------
def binarySearchSA(T, P, SA, l, r):
    assert l != -1 and r != -1
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
    assert l != -1 and r != -1
    if r <= l: return (l, r)
    c = (l + r) // 2
    assert c == l + (r - l) // 2
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
# Zuffification # TODO clean me and most importantly FIX ME
#-------------------------------------------------------------------------------
def dfs(T, LCP, CT, i, j, depth=0):
    if j - i <= 1: return # leaves have no children
    l = i
    if i < CT[j-1] < j:
        r = CT[j-1]
    else:
        r = CT[i]
    d = LCP[r]
    while l < r:
        print(f"{' '*depth}[{l}, {r})")
        sleep(1)
        dfs(T, LCP, CT, l, r, depth=depth+1)
        l = r
        if LCP[r] != LCP[CT[r]] or LCP[r] > LCP[r+1]:
            print(f"{' '*depth}[{l}, {j})")
            sleep(1)
            dfs(T, LCP, CT, l, j, depth=depth+1)
            break
        r = CT[r]

def zca(T, SA, LCP, CT):
    Z = dict()
    zfill(Z, T, SA, LCP, CT, 0, len(T), 0)
    return Z

def zfill(Z, T, SA, LCP, CT, i, j, name_len):
    if j - i <= 1: return # leaves have no children
    l = i
    if i < CT[j-1] < j:
        r = CT[j-1]
    else:
        r = CT[i]
    d = LCP[r]  # lcp value
    # ZFILL
    extent_len = d
    handler_len = twoFattestLR(name_len, extent_len)
    handler = T[SA[i] : SA[i] + handler_len + 1]
    Z[handler] = (i, j)
    #########
    while True: # repeat until => do while not
        zfill(Z, T, SA, LCP, CT, l, r, extent_len + 1)
        l = r
        r = CT[r]
        if LCP[l] != LCP[CT[l]] or LCP[l] > LCP[l+1]: break
    zfill(Z, T, SA, LCP, CT, l, j, extent_len + 1)

# sembrerebbe funzionare
def exitNode(T, S, SA, LCP, CT, i, j):
    namelen = 1 + max(LCP[i], LCP[j])
    #assert T[SA[i] : SA[i] + limit] == S[0 : limit] # WARNING questa assert di merda è da rivedere
    extentlen = len(T) - SA[i] if j - i == 1 else getlcp(i, j, LCP, CT) # it is always an internal node
    print(f"i, j = {i, j}")
    print(f"T = {T}")
    print(f"S = {S}")
    print(f"namelen = {namelen}")
    print(f"extentlen = {extentlen}")
    print(f"name = {T[SA[i] : SA[i] + namelen]}")
    print(f"extent = {T[SA[i] : SA[i] + extentlen]}")
    print(f"compacted = {T[SA[i] + namelen : SA[i] + extentlen]}")
    print(f"portion = {S[namelen : extentlen]}")
    name = T[SA[i] : SA[i] + namelen]
    assert name == S[: len(name)]
    compacted = T[SA[i] + namelen : SA[i] + extentlen]
    portion = S[namelen : extentlen]
    # if portion == "": return (1, 0)  # TODO ERRORE QUI
    if compacted[: min(len(compacted), len(portion))] != portion[: min(len(compacted), len(portion))]:
        print("string not found")
        return (1, 0) # string not found
    if extentlen < len(S):
        (l, r) = getChild(T, SA, LCP, CT, i, j, S[extentlen])
        print(f"getChild(T, SA, LCP, CT, i, j, S[extentlen]) => {i}, {j}, {S[extentlen]} => {(l, r)}")
        if r < l: return (1, 0)
        return exitNode(T, S, SA, LCP, CT, l, r)
    return (i, j)

def fatBinarySearch(T, S, SA, LCP, CT, Z):
    alphai, alphaj = 0, len(T)
    l, r = 0, len(S)
    while l < r:
        f = twoFattestLR(l+1, r)
        betal, betar = Z.get(S[0 : f], (1, 0))
        print(f"l: {l}, r: {r}, f: {f}")
        print(f"handle = {S[0 : f]}")
        print(f"beta = {(betal, betar)}")
        if betal < betar:
            l = getlcp(betal, betar, LCP, CT) + 1 # it is always an internal node
            assert l >= f
            print(f"new l: {l}")
            #l = len(T) - SA[betal] if betar - betal == 1 else getlcp(betal, betar, LCP, CT) # it is always an internal node
            alphai, alphaj = betal, betar
        else:
            r = f - 1
    return (alphai, alphaj)

def zuffixStringSearch(T, P, SA, LCP, CT, Z):
    i, j = fatBinarySearch(T, P, SA, LCP, CT, Z)
    return exitNode(T, P, SA, LCP, CT, i, j)
    #return exitNode(T, P, LCP, CT, 0, len(T))
#-------------------------------------------------------------------------------


#  #-------------------------------------------------------------------------------
#  # Test: ttatctctta
#  #-------------------------------------------------------------------------------
#  T = 'ttatctctta'
#  SA = saca(T)
#  LCP = lcpca(T, SA)
#  UP, DOWN, NEXT = udnca(LCP)
#  CT = ctca(LCP)
#  assert CT == ctca3(LCP, UP, DOWN, NEXT)
#  Z = zca(T, SA, LCP, CT)
#  # print me if you want
#  #-------------------------------------------------------------------------------
#
#
#  #-------------------------------------------------------------------------------
#  # Test: abracadabra~
#  #-------------------------------------------------------------------------------
#  T = 'abracadabra~'
#  SA = saca(T)
#  LCP = lcpca(T, SA)
#  UP, DOWN, NEXT = udnca(LCP)
#  CT = ctca(LCP)
#  assert CT == ctca3(LCP, UP, DOWN, NEXT)
#  Z = zca(T, SA, LCP, CT)
#  # print me if you want
#  #-------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------
# Test: bug
#-------------------------------------------------------------------------------
# T = 'bbbcabbcc~'
# P = 'bbcbbcbac'
# SA = saca(T)
# LCP = lcpca(T, SA)
# UP, DOWN, NEXT = udnca(LCP)
# CT = ctca(LCP)
# assert CT == ctca3(LCP, UP, DOWN, NEXT)
# Z = zca(T, SA, LCP, CT)
#
# enhancedStringSearch(T, P, SA, LCP, CT)
# exitNode(T, P, SA, LCP, CT, 0, len(T))
# fatBinarySearch(T, P, SA, LCP, CT, Z)
# zuffixStringSearch(T, P, SA, LCP, CT, Z)
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
        print("START HERE")
        print(f"T = {T}")
        print(f"P = {P}")
        SA = saca(T)
        LCP = lcpca(T, SA)
        UP, DOWN, NEXT = udnca(LCP)
        CT = ctca(LCP)
        Z = zca(T, SA, LCP, CT)
        assert CT == ctca3(LCP, UP, DOWN, NEXT)
        (l1, r1) = binarySearchSA(T, P, SA, 0, len(T)-1)
        (l2, r2) = acceleratedBinarySearchSA(T, P, SA, 0, len(T)-1, 0, 0)
        (l3, r3) = enhancedStringSearch(T, P, SA, LCP, CT)
        (l4, r4) = enhancedStringSearch3(T, P, SA, LCP, UP, DOWN, NEXT)
        (l5, r5) = zuffixStringSearch(T, P, SA, LCP, CT, Z)
        print(f'zuffixStringSearch("{T}", "{P}", {SA}, {LCP}, {CT}, {Z})')
        print(f"Z = {Z}")
        print(f"result = {zuffixStringSearch(T, P, SA, LCP, CT, Z)}")
        if l1 >= r1 or l2 >= r2 or l3 >= r3 or l4 >= r4 or l5 >= r5:
            assert l1 >= r1 and l2 >= r2 and l3 >= r3 and l4 >= r4 and l5 >= r5, f'Expected: {l1} >= {r1} and {l2} >= {r2} and {l3} >= {r3} and {l4} >= {r4} and {l5} >= {r5}'
        else:
            assert l1 == l2 == l3 == l4 == l5 and r1 == r2 == r3 == r4 == r5, f'Expected: {l1} == {l2} == {l3} == {l4} == {l5} and {r1} == {r2} == {r3} == {r4} == {r5}'
            assertMatch(T, P, SA, l1, r1)
            cnt += 1
    return cnt

# for _ in range(100): testRandom(10)
# for _ in range(100): testRandom(100)
# for _ in range(100): testRandom(127)
#-------------------------------------------------------------------------------

def getParent(T, SA, LCP, CT):
    stack = [ [0, 0, len(T)] ]
    for i in range(1, len(T)):
        lb = i - 1
        while LCP[i] < stack[-1][0]:
            stack[-1][2] = i
            interval = stack.pop()
            print(f'{interval[0]}-[{interval[1]} .. {interval[2]})')
            lb = interval[1]
        if LCP[i] > stack[-1][0]:
            stack.append([LCP[i], lb, i])
        print(stack)

getParent(T, SA, LCP, CT)


def fibonacci(n):
    prec, curr = 'a', 'ab'
    for i in range(n):
        prec, curr = curr, curr+prec
    return curr


def fiblen(n):
    prec, curr = 1, 2
    for i in range(n):
        prec, curr = curr, curr+prec
    return curr

T = fibonacci(10)
SA = saca(T)
LCP = lcpca(T, SA)
UP, DOWN, NEXT = udnca(LCP)
CT = ctca(LCP)
assert CT == ctca3(LCP, UP, DOWN, NEXT)
Z = zca(T, SA, LCP, CT)
