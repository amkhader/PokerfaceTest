# Updating time stamps for subtitles
import re
import datetime

f = open ('Keine Chance-TEDxTUHH.en.srt', 'r')

new = open ('Keine Chance-TEDxTUHH.en.new.srt', 'w')

new_t1 = 0
new_t2 = 0
##firstt1_s =""
##firstt1_ms =""

counter = 0

##for line in f:
##    if re.match("\d\d:\d\d",line[0:5]):
##        firstt1_h = line[0:2]
##        firstt1_m = line[3:5]
##        firstt1_s = line[6:8]
##        firstt1_ms = line[9:12]
##        break
##print 'START : ' + firstt1_s + ',' + firstt1_ms


for line in f:
    #if line == "%d%d:%d%d:%d%d,%d+ --> %d%d:%d%d:%d%d,%d+ \n":
    if re.match("\d\d:\d\d",line[0:5]):
        if counter == 0 :      
            firstt1_h = line[0:2]
            firstt1_m = line[3:5]
            firstt1_s = line[6:8]
            firstt1_ms = line[9:12]
            firstt1 = (((((float(firstt1_h) * 60) + float(firstt1_m)) * 60) + float(firstt1_s)) * 1000) + float(firstt1_ms)
            print firstt1
            #print 'START : ' + firstt1_s + ',' + firstt1_ms
        oldt1_h = line[0:2]
        oldt1_m = line[3:5]
        oldt1_s = line[6:8]
        oldt1_ms = line[9:12]

        oldt2_h = line[17:19]
        oldt2_m = line[20:22]
        oldt2_s = line[23:25]
        oldt2_ms = line[26:-1]
        #print 'CONT : ' + oldt1_s + ',' + oldt1_ms
##        print oldt1_s + ',' + oldt1_ms
##        print oldt2_s + ',' + oldt2_ms

        intert1 = (((((float(oldt1_h) * 60) + float(oldt1_m)) * 60) + float(oldt1_s)) * 1000) + float(oldt1_ms)
        intert2 = (((((float(oldt2_h) * 60) + float(oldt2_m)) * 60) + float(oldt2_s)) * 1000) + float(oldt2_ms)

        newt1 = (intert1 - firstt1)
        newt1_s = int(float(newt1)/1000.0) #2
        newt1_ms = ((float(newt1)/1000.0) - newt1_s) * 1000 #26
        newt1_m = (newt1_s/60) #0
        newt1_s = ((float(newt1_m)/60) - newt1_m) * 60 #2
        newt1_h = (newt1_m/60) #0
        newt1_m = ((float(newt1_h)/60) - newt1_h) #0 

        print newt1
        print str(int(newt1_h)) + ':' + str(int(newt1_m)) + ':' + str(int(newt1_s)) + ',' + str(int(newt1_ms))
        newt2 = (intert2 - firstt1)


##        newt1_h = int(oldt1_h) - int(firstt1_h)
##        newt1_m = int(oldt1_m) - int(firstt1_m) 
##        newt1_s = int(oldt1_s) - int(firstt1_s)
##        newt1_ms = int(oldt1_ms) - int(firstt1_ms)
##
##        newt2_h = int(oldt2_h) - int(firstt1_h)
##        newt2_m = int(oldt2_m) - int(firstt1_m)
##        newt2_s = int(oldt2_s) - int(firstt1_s)
##        newt2_ms = int(oldt2_ms) - int(firstt1_ms)

        counter += 1
        
##        print str(newt1_h) + ":" + str(newt1_m) + ":" + str(newt1_s) + ',' + str(newt1_ms)
##        print str(newt2_h) + ":" + str(newt2_m) + ":" + str(newt2_s) + ',' + str(newt2_ms)
