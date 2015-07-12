import csv

## editing features from different intervals
## returns all values for each channel
## each channel has one row

for i in range(3):
    i += 1
    x = 2**i

    f = open('interval_m_' + str(x) + '.csv', 'r')

    new = open ('parsed features.csv', 'w')

    counter = 0

    for line in f:
        counter += 1
        if counter % 256 == 0:
            new.write('\n')
        else:
            new.write(line[0:-2] + ",")
