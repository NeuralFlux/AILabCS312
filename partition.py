import sys

l = list(range(int(sys.argv[1])))
partition = []
flags = [False] * len(l)
while True:
    a = [l[i] for i, flag in enumerate(flags) if flag]
    b = [l[i] for i, flag in enumerate(flags) if not flag]
    partition.append([a, b])

    for i in range(len(l)):
        flags[i] = not flags[i]
        if flags[i]:
            break
    else:
        break
