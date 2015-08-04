#!/usr/bin/python

import sys
import json
import linecache
import time
import datetime
import calendar

# This file takes two different csv files and parses them to get the data in python format


def usage():
	print( sys.argv[0] + " eyetracking.csv  subtitles_template.csv")
	exit(-1)


def parseEye(eyefile):

	file= open(eyefile)

	data=[] #initialize empty datastructure
	for line in file:
		
		(x, y, fixation, timestamp) = line.strip().split(",") # Assuming the file has 3 columns per line. Fix this
		dictionary={'timestamp':float(timestamp),'x':float(x),'y':float(y), 'fixation': str(fixation)} # We save the information in a dictionary data structure
		data.append(dictionary) # We save the current

	return data

def binarySearch(array,x): #Assumes sorted array.
	first=0
	last =len(array) -1
	found=False
	foundPos=-1

	#fill the details to do the binary search here

	return foundPos

def parseSub(subfile):
	file = open(subfile)
	data={}
	list1 = []

	file.readline()
	for line in file:
		(subID, wordAltered, wordID, word, altClass, wordShown, starttime, endtime, top, left, newTop, newLeft, width, height) =  line.strip().split(",") 
		subID = str(subID)
		wordID = str(wordID)
		word = str(word)
		altclass = str(altClass)
		starttime=float(starttime)
		endtime=float(endtime)
		top=float(newTop) ##the new transformed top/left coordinates that actually match with the subs.
		left=float(newLeft)
		height = float(height)
		width = float(width)



		if starttime in data:
			data[starttime]['words'].append({'word':word, 'class':altclass, 'top':top, 'left': left, 'width':width, 'height': height,'endtime':endtime})
		else:
			data[starttime]={'words': [{'word':word, 'class':altclass, 'top':top, 'left': left, 'width':width, 'height': height,'endtime':endtime}] }
	s = sorted(data.keys())
	for key in s:
		dict1 = {key: data[key]}
		list1.append(dict1)
	return list1

def timeCol(eyefile, subfile):
        assc={}
        tE = parseEye(eyefile)
        tS = parseSub(subfile)
        timecol = []
        fixEnd = 0        
        fixStart = fixEnd
        duration = 0
        final = []
        gaze = []
        eegclass = ""
        c = 0
        for j in range(len(tS)):
                for key in (tS[j]):
                        for word in tS[j][key]['words']:
                                for i in range (len(tE)):
                                        if (tE[i].get('timestamp')) >= key and (tE[i].get('timestamp')) <= word.get('endtime'):
                                                assc = {'eyeTime': (tE[i].get('timestamp')), 'subStartTime':key}
                                                timecol.append(assc)
                                                if (tE[i].get('x') > word.get('left') and tE[i].get('x') < (word.get('left') + word.get('width')) and (tE[i].get('y') > word.get('top') and tE[i].get('y') < (word.get('top') + word.get('height')))):
                                                             dictionary = {'word': word.get('word'), 'timestamp': tE[i].get('timestamp'), 'altclass':word.get('class')}
                                                             fixEnd = tE[i].get('timestamp')
                                                             duration = fixEnd - fixStart
                                                             print (key)
                                                             if len(final) != 0:
                                                                        if final[-1].get('word') != word.get('word'):

                                                                             gaze.append([final[-1].get('word'), fixStart, duration, final[-1].get('altclass')])
                                                                             fixStart = tE[i].get('timestamp')
                                                                             ##print (fixStart)
                                                                        final.append(dictionary)
                                                             else:
                                                                        final.append(dictionary)
                                                                        fixStart = final[0].get('timestamp')
                                                    
                                                        
        swords= set(("the","in","a","an","on","to","of","that"))
        f = open("test", "w") ## change file for each test
        for i in range(len(gaze)):
                if gaze[i][0] not in swords:
                        f.write("%s, %d,%.1f,%s\n"% (gaze[i][0],gaze[i][1],gaze[i][2],gaze[i][3]))
        f.close()
        return gaze
               

#Sample transform function, build your own here
def transformCoordinates(x,delta=0,factor=1):
	newx = (x*factor)+delta
	return newx

def detectWords(eyedata,subdata):

	words=[]

	# Your collision detection algorithm goes here

	return words 



def test():
	main("Demo2Eyenew.csv","Ferda-Video2.csv")
	timeCol("Demo2Eyenew.csv","Ferda-Video2.csv")

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



	if (len(sys.argv)!=3): # Check that we have two arguments
		test()
		usage()

	eyefile = sys.argv[1] # Eyetracking file
	subfile = sys.argv[2] # Template file
	main(eyefile,subfile)


