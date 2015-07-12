# interval feature extraction
import math

T = 10
b = 1
e = 2
interval = 2
add = 0
mean = 0
v = 0
sd = 0
cov = 0
cv = 0

while interval <= T:
    while e <= T:
        for i in range (b,e+1):
            add += 5 #should add value x_i , not 5
            i+=1
        mean = (add/interval)
        add = 0
        print 'mean : ' + str(mean)
        for i in range (b,e+1):
            v += 5**2 #should add value x_i , not 5
            i+=1
        sd = math.sqrt(v - (mean**2))
        v = 0
        print 'standard deviation : ' + str(sd)
        for i in range (b,e+1):
            cv += (i * 5) - (mean * ((b + e)/2))
            i += 1
        cov = cv/i
        cv = 0
        print 'covariance : ' + str(cv)
        b += 1
        e += 1
    interval *= 2
    b = 1
    e = interval
