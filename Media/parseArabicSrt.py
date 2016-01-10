import codecs

file = codecs.open('PhasesofMeiosisI.ar.srt','r', encoding = 'utf-8')


fileOut = codecs.open('PhasesofMeiosisI.txt','w', encoding = 'utf-8')

count = 0

for line in file:
    if (line[0] not in ['1','2','3','4','5','6','7','8','9','0']):
        if not line.strip() :
            count +=1
        else:
            fileOut.write(str(count) + " " +line)

print count
file.close
