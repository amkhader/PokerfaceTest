#!/usr/bin/python


import sys
import json

# This file takes two different csv files and parses them to get the data in python format
# Modify it to get the expected results.


def usage():
	print sys.argv[0] + " eyetracking.csv  subtitles_template.csv"
	exit(-1)


def parseEye(eyefile):

	file= open(eyefile)

	data=[] #initialize empty datastructure
	for line in file:
		# TODO: Fill here the details:
		
		(timestamp, eyex, eyey) = line.strip().split(",") # Assuming the file has 3 columns per line. Fix this
		dictionary={'timestamp':int(timestamp),'x':float(eyex),'y':float(eyey)} # We save the information in a dictionary data structure
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
		# We have a data structure order by starttime
		# +-{start_time1}-+--{word1}
		# |				  +--{word2}
		# |				  +--{word3}
		# |
		# +-{start_time2}-+--{word4}
		#TODO: fill in the details

	for line in file:
		(timemin, timemax, word, top,left, width, heigth) =  line.strip().split(",")# Assuming we have 5 columns, fix this
		timemin=int(timemin)
		timemax=int(timemax)
		top=float(top)
		left=float(left)
		width=float(width)
		heigth=float(width)

		if timemin in data:
			data[timemin]['words'].append({'word':word,'top':top, 'left': left, 'width':width, 'heigth': heigth,'timemax':timemax}) 
		else:
			data[timemin]={'timemax':timemax,'words': [{'word':word,'top':top, 'left': left, 'width':width, 'heigth': heigth,'timemax':timemax}] }


	return data

#Sample transform function, build your own here
def transformCoordinates(x,delta=0,factor=1):
	newx = (x*factor)+delta
	return newx

def detectWords(eyedata,subdata):

	words=[]

	# Your collision detection algorithm goes here

	return words 



def test():
	main("test/simpleeye.csv","test/simplesub.csv")


def main(eyefile,subfile):
	eyedata=parseEye(eyefile)
	subdata=parseSub(subfile)
	words=detectWords(eyedata,subfile)

	print "Detected observed words= ", words

if __name__ == '__main__':
	


	if (len(sys.argv)!=3): # Check that we have two arguments
		test()
		usage()

	eyefile = sys.argv[1] # Eyetracking file
	subfile = sys.argv[2] # Template file
	main(eyefile,subfile)


