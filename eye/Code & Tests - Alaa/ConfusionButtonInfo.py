from detectWords import parseSub
from detectWords import parseSub
from detectWords import timeCol

def parseConfusions(confusionfile):
    file = open(confusionfile)
    confusionTimeList = []
    for time in file:
        if time[-1] == '\n' :
            confusionTimeList.append(time[0:-1])
        else:
            confusionTimeList.append(time)
    return confusionTimeList

def parseSubSentence(subfile):
    SubList = parseSub(subfile)

    sentence = {} #has starttime and list of words

    AllSentences = []

    subline = [] #list of all words and their data

    for j in range(len(SubList)): #going through subtitles list
                for key in (SubList[j]): #going through dictionary in list index
                        for data in SubList[j][key]['subline']:
                                subline.append(data)

    sentence = {'starttime': subline[0]['starttime'],'endtime' : subline[0]['endtime'] ,'sentence': [subline[0]['word'][1:-1]]}
    for k in range(1, len(subline)):
        if(subline[k]['starttime'] == subline[k-1]['starttime']):
            sentence['sentence'].append(subline[k]['word'][1:-1])
        else:
            AllSentences.append(sentence)
            sentence = {'starttime': subline[k]['starttime'], 'endtime' : subline[k]['endtime'],'sentence': [subline[k]['word'][1:-1]]}

    return AllSentences

def findSubLines(subfile, confusionfile):
    SubSentenceList = parseSubSentence(subfile)
    confusionList = parseConfusions(confusionfile)

    confusionData = {} #dictionary of confusion to sentences
    ListConfData = [] #List of confusionData dictionaries

    
    for k in range(len(SubSentenceList)):
        for i in range (len(confusionList)):
            #retrieve time collisions
            if (float(confusionList[i]) >= SubSentenceList[k]['starttime'] and  float(confusionList[i])<= SubSentenceList[k]['endtime']):
                #storing time collisions
                if (k == 0):
                    confusionData = {'ConfTime': float(confusionList[i]), 'subStartTime': SubSentenceList[k]['starttime'], 'sentence1':SubSentenceList[k]['sentence']
                                 , 'sentence2' : None , 'sentence3' : None}
                if (k == 1):
                    confusionData = {'ConfTime': float(confusionList[i]), 'subStartTime': SubSentenceList[k]['starttime'], 'sentence1':SubSentenceList[k]['sentence']
                                 , 'sentence2' : SubSentenceList[k-1]['sentence'], 'sentence3' : None}

                else:
                    confusionData = {'ConfTime': float(confusionList[i]), 'subStartTime': SubSentenceList[k]['starttime'], 'sentence1':SubSentenceList[k]['sentence']
                                     , 'sentence2' : SubSentenceList[k-1]['sentence'], 'sentence3' :SubSentenceList[k-2]['sentence']}
                ListConfData.append(confusionData)



    ### adding sentence to file to score ###
    file = open("scoreJargon", 'w')
    
    for x in ListConfData:
        for key in x:
##            if x[key]!= None and key!= 'ConfTime' and key != 'subStartTime':
##                for word in x[key]:
##                    file.write(word + "\n")
            if x[key]!= None and key!= 'ConfTime' and key != 'subStartTime':
                file.write(" ".join(x[key]) +"\n")
                
    file.close()
    return ListConfData


def findLastWord(eyefile, subfile, confusionfile):
    eyeCol = timeCol(eyefile, subfile)
    confusionList = parseConfusions(confusionfile)
    LastWord = []

    for time in confusionList:
        for k in range(1, len(eyeCol)):
            if(eyeCol[k]['timestamp'] >= float(time) and eyeCol[k-1]['timestamp'] <= float(time)):
                print eyeCol[k-1]['word']
                LastWord.append((float(time), eyeCol[k-1]['word']))
                
    #might not have looked at anything after last confusion
                
    lastConf = False
    for (time, word) in LastWord:
        if time == float(confusionList[-1]):
            lastConf = True

    if lastConf == False:    
        LastWord.append((float(confusionList[-1]),eyeCol[-1]['word']))
    return LastWord
    

if __name__ == '__main__':

    #print parseConfusions('ConfusionFileTest.txt')
    #print parseSubSentence('ConfusionButtonSub.txt')

    findSubLines('Alaa_jargonSubs_1', 'Alaa_jargon_button_1')
    
##    for x in findSubLines('ConfusionButtonSub.txt', 'ConfusionFileTest.txt'):
##        for key in x:
##            if x[key]!= None and key!= 'ConfTime' and key != 'subStartTime':
##                print " ".join(x[key])
        


##    print findLastWord('Alaa_Test2_button_eye.csv','ConfusionButtonSub2',
##                       'ConfusionButtonPress')
##                       
