
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


        sentence = {} #has starttime and list of words

        AllSentences = []

        subline = [] #list of all words and their data

        for j in range(len(list1)): #going through subtitles list
                    for key in (list1[j]): #going through dictionary in list index
                            for data in list1[j][key]['subline']:
                                    subline.append(data)

        sentence = {'starttime': subline[0]['starttime'],'endtime' : subline[0]['endtime'] ,'sentence': [subline[0]['word'][1:-1]]}
        for k in range(1, len(subline)):
            if(subline[k]['starttime'] == subline[k-1]['starttime']):
                sentence['sentence'].append(subline[k]['word'][1:-1])
            else:
                AllSentences.append(sentence)
                sentence = {'starttime': subline[k]['starttime'], 'endtime' : subline[k]['endtime'],'sentence': [subline[k]['word'][1:-1]]}


        file = open("scoreJargon", 'w')
        
        for x in AllSentences:
            for key in x:
                if key == 'sentence':
                    for i in range (len(x[key])):
                        if x[key][i][-3:] == "...":
                            x[key][i] = x[key][i][0:-3]
                            
                        if x[key][i][-1] == "." or x[key][i][-1] == ",":
                            x[key][i] = x[key][i][0:-1]
                        if x[key][i][0] == '"':
                            x[key][i] = x[key][i][1:]
                    print " ".join(x[key]) +"\n"
                    file.write(" ".join(x[key]) +"\n")
        file.close()


print parseSub('Alaa_jargonSubs_1')
    
