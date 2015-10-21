#!/usr/bin/python

import sys
import json
import linecache
import time
import datetime
import calendar

# This file takes two different csv files and parses them to get the data in python format
# Pass eye file and sub file as arguments
# Getting time and space collisions - words looked at and how long they're looked at

def usage():
	print( sys.argv[0] + " eyetracking.csv  subtitles_template.csv")
	exit(-1)



#parses eye file 
def parseEye(eyefile):

	file= open(eyefile)

	data=[] #initialize empty datastructure
	        #list of dictionaries
	        #each line is a dictionary
	
	for line in file:
		(x, y, fixation, timestamp) = line.strip().split(",") #timestamp in milliseconds
		# We save the information in a dictionary data structure
		dictionary={'timestamp':int(timestamp[2:-1]),'x':float(x),'y':float(y)} #, 'fixation': str(fixation)
		data.append(dictionary) # We save the current

	return data



## returns a list of dictionaries. 
## each dictionary is a line from the subtitles.
def parseSub(subfile):
        file = open(subfile)
        data={} #dictionary
        list1 = []

        file.readline () #skip line with headlines
	
        for line in file:
                (subID, wordAltered, wordID, word, altClass, wordShown, starttime, endtime, top, left, newTop, newLeft, width, height) =  line.strip().split(",") 
                subID = str(subID)
                wordID = str(wordID)
                word = str(word)
                altclass = str(altClass)
                starttime= int(starttime[1:-1])
                endtime= float(endtime[1:-1])
                top= float(newTop[1:-1]) ##the new transformed top/left coordinates that actually match with the subs
                left= float(newLeft[1:-1])
                height = float(height[1:-1])
                width = float(width[1:-1])


                # data is a dictionary of key being starttime and values are all the words in one subtitle line
                # values are lists with the word and other info (class,top,left, etc...)

                if starttime in data:
                        #words from line already in dictionary in a list
                	data[starttime]['subline'].append({'word':word, 'class':altclass, 'top':top, 'left': left,
                                                           'width':width, 'height': height,'starttime':starttime, 'endtime':endtime})
                else:
                        #new subtitle line
                	data[starttime]={'subline': [{'word':word, 'class':altclass, 'top':top, 'left': left,
                                                      'width':width, 'height': height,'starttime':starttime,'endtime':endtime}] }
                	
        s = sorted(data.keys()) # sorting start times 
        for key in s:
        	dict1 = {key: data[key]} #dictionary with key value pair (all words shown at same starttime in order)
        	list1.append(dict1)
        return list1 



## returns time colisions - find when eye timestamps match subtitle timestamps
## return type: list of dictionaries
def timeCol(eyefile, subfile):

        subline = [] #list of all words and their data
        
        assc={} #dictionary pair of (eye tracking timestamp, subtitle starttime)
        dictionary={}
        tE = parseEye(eyefile)
        tS = parseSub(subfile)
        timecol = [] #list of assc dictionaries
        fixEnd = 0        
        fixStart = fixEnd
        duration = 0
        final = []
        gaze = []
        eegclass = ""
        c = 0
        result = []
        for j in range(len(tS)): #going through subtitles list
                for key in (tS[j]): #going through dictionary in list index
                        for data in tS[j][key]['subline']:
                                subline.append(data)

        for k in range(len(subline)):
                word = subline[k]['word']
                for i in range (len(tE)):
                        #print tE[i]['timestamp']
                        #print subline[k]['starttime']

                        #retrieve time collisions
                        if (tE[i].get('timestamp')) >= subline[k]['starttime'] and (tE[i].get('timestamp')) <= subline[k]['endtime']:
                                #storing time collisions
                                assc = {'eyeTime': (tE[i].get('timestamp')), 'subStartTime': subline[k]['starttime']}
                                timecol.append(assc)

                                #going through time collisions to find space collisions
                                
                                if (tE[i].get('x') > subline[k]['left'] and tE[i].get('x') < (subline[k]['left'] + subline[k]['width']) and
                                   (tE[i].get('y') > subline[k]['top'] and tE[i].get('y') < (subline[k]['top'] + subline[k]['height']))):
                                        
                                        #saving time and space collision with a timstamp and the subtitle class 
                                        dictionary = {'word': subline[k]['word'], 'timestamp': tE[i].get('timestamp'), 'altclass':subline[k]['class']}
                                        result.append(dictionary);
                                        #print dictionary
        return result

##                                     fixEnd = tE[i].get('timestamp')
##                                     duration = fixEnd - fixStart
##                                     print (key)
##                                     if len(final) != 0:
##                                                if final[-1].get('word') != word.get('word'):
##
##                                                     gaze.append([final[-1].get('word'), fixStart, duration, final[-1].get('altclass')])
##                                                     fixStart = tE[i].get('timestamp')
##                                                     ##print (fixStart)
##                                                final.append(dictionary)
##                                     else:
##                                                final.append(dictionary)
##                                                fixStart = final[0].get('timestamp')
                                                    
                                                        
##        swords= set(("the","in","a","an","on","to","of","that"))
##        f = open("test", "w") ## change file for each test
##        for i in range(len(gaze)):
##                if gaze[i][0] not in swords:
##                        f.write("%s, %d,%.1f,%s\n"% (gaze[i][0],gaze[i][1],gaze[i][2],gaze[i][3]))
##        f.close()
##        return gaze



#Takes time and space collisions and smooths them with a threshold
#returns a list of tuples (word, duration)
#duration is milliseconds
def smooth(colls, threshold):
        result = []
        counter = 0 #counts number of a certain word seen so far
        firstSeen = colls[0]
        firstWord = firstSeen.get('word')
        next = ""
        for i in range (len(colls)):
                next = colls[i].get('word')
                
                if (next == firstWord):
                        counter+=1
                else:
                        if (counter >= threshold):
                                lastSeen = colls[i-1]
                                duration = lastSeen.get('timestamp') - firstSeen.get('timestamp')
                                result.append((firstWord, duration))
                                counter = 1
                                firstSeen = colls[i]
                                firstWord = firstSeen.get('word')
                        else: #don't meet threshold (shouldn't be included)
                                counter = 1
                                firstSeen = colls[i]
                                firstWord = firstSeen.get('word')
        #adding last word to list
        word = colls[(len(colls))-1].get('word')
        duration = colls[(len(colls))-1].get('timestamp') - colls[(len(colls))-counter].get('timestamp')
        result.append((word, duration))
        return result


#Sample transform function, build your own here
def transformCoordinates(x,delta=0,factor=1):
	newx = (x*factor)+delta
	return newx





def test():
	main("Demo2Eyenew.csv","Ferda-Video2.csv")
	smooth(timeCol("Demo2Eyenew.csv","Ferda-Video2.csv"), 2)

def main(eyefile,subfile):
   print("Going to EYE")
   eyedata=parseEye(eyefile)
   #print(eyedata)
   print("Going to SUB")
   ##subdata=parseSub(subfile)
   #print(subdata)
   ##words=timeCol(eyefile,subfile)
   #print("Going to COL")
   #print(str(words) + "not printing")

	##print ("Detected observed words= ", words)

if __name__ == '__main__':

        #print "test"
        print smooth(timeCol("Alaa_Test1.csv", "Alaa_Test1"),2)

##	if (len(sys.argv)!=3): # Check that we have two arguments
##		test()
##		usage()
##
##	eyefile = sys.argv[1] # Eyetracking file
##	subfile = sys.argv[2] # Template file
##	main(eyefile,subfile)


