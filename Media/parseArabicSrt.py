import codecs

file = codecs.open('PhasesofMeiosisI.ar.srt','r', encoding = 'utf-8')


fileOut = codecs.open('PhasesofMeiosisI.txt','w', encoding = 'utf-8')

for line in file:
    if (line[0] not in ['1','2','3','4','5','6','7','8','9','0']):
        fileOut.write(line)

file.close
