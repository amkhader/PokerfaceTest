import csv

f = open('co2a0000364.rd.000', 'r')

#will create a .csv file with each ROW being a channel (easier)

new = open ('parsed channels.csv', 'w')

counter = 0

for line in f:
    if line[0] != "#":
        counter += 1
        if counter % 256 == 0:
            new.write(line[-7:-1] + '\n')
        else:
            new.write(line[-7:-1] + ",")
        #print line

##with open('parsed channels.csv', "wb") as csv_file:
##    writer = csv.writer(csv_file, delimiter=',')
##    for line in f:
##        if line[0] != "#":
##            writer.writerow(line)
