from numpy import sqrt
import sys

totalframes = int(sys.argv[1])
maxsweeps = int(sys.argv[5])

nav = [0.0] * (maxsweeps + 1)      # sum of n(t)
nav2 = [0.0] * (maxsweeps + 1)     # sum of n(t)^2
area_under = [0.0] * (maxsweeps + 1)  # sum of Area(t)

for i in range(totalframes):
    fname = "rand_" + str(i + 1) + ".dat"
    with open(fname, 'r') as f:
        for line in f:
            l = line.split()
            if len(l) >= 4 and l[0] == "t" and l[2] == "NANT":
                t = int(l[1])
                if 0 <= t <= maxsweeps:
                    n = float(l[3])
                    nav[t] += n
                    nav2[t] += n * n

    afname = "PolymerArea_" + str(i + 1) + ".txt"
    with open(afname, 'r') as f:
        for line in f:
            l = line.split()
            if len(l) >= 4 and l[0] == "t" and l[2] == "Area":
                t = int(l[1])
                if 0 <= t <= maxsweeps:
                    area_under[t] += float(l[3])

with open("AverageOverSimulations.txt", 'w') as f:
    print("t\tn\tSD", file=f)
    for t in range(maxsweeps + 1):
        nav[t] = nav[t] / float(totalframes)
        nav2[t] = sqrt(nav2[t] / float(totalframes) - nav[t] * nav[t])
        print(t, "\t", nav[t], "\t", nav2[t], file=f)

with open("PolymerArea.txt", 'w') as f:
    print("t\tArea", file=f)
    for t in range(maxsweeps + 1):
        print(t, "\t", area_under[t] / float(totalframes), file=f)
