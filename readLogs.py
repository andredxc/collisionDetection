#!/bin/python
import re
import os

class Injection:
    """
    Stores information about a single fault injection
    """
    def __init__(self, varName, varType, line, memContentBefore, newValue, faultModel, fileName, functionName):
        self.varName = varName
        self.varType = varType
        self.line = line
        self.memContentBefore = memContentBefore
        self.newValue = newValue
        self.faultModel = faultModel
        self.fileName = fileName
        self.functionName = functionName

    def __repr__(self):

        if self.varType.find("structure") > -1 or self.varType.find("array") > -1:
            # Doesn't print values for arrays or structs
            return "varName: %s\nvarType: %s\nline: %s\nfaultModel: %s\nfileName: %s\nfunctionName: %s\n" %(self.varName, self.varType, self.line, self.faultModel, self.fileName, self.functionName)
        else:
            return "varName: %s\nvarType: %s\nline: %s\nmemCont: %s\nnewValue: %s\nfaultModel: %s\nfileName: %s\nfunctionName: %s\n" %(self.varName, self.varType, self.line, self.memContentBefore, self.newValue, self.faultModel, self.fileName, self.functionName)
        #return "%s <= %s at %s:%s" %(self.varName, self.newValue, self.functionName, self.line)


#-------------------------------------------------------------------------------
def main():

    logDir = "/home/andre/ufrgs/ftf/final_logs/collisionDetection-random/"

    for fileName in os.listdir(logDir):
        # Skip regular files
        if not os.path.isdir(logDir + fileName):
            continue

        if fileName == "failed-injection":
            # Failed
            continue

        elif fileName == "hangs":
            # Hangs
            hangList = handleHangs(logDir+"hangs/")

        elif fileName == "masked":
            # Masked
            continue

        elif fileName == "noOutputGenerated":
            # No output
            continue

        elif fileName == "sdcs":
            # SDCs
            continue

    printList(hangList)

#-------------------------------------------------------------------------------
def handleHangs(hangDir):

    hangList = list()

    for dateDir in os.listdir(hangDir):

        if not os.path.isdir(hangDir+dateDir):
            # Skip regular files
            continue
        # Accesses dirs containing actual log files
        for logDir in os.listdir(hangDir+dateDir):
            # Finds the useful log file
            logFileList = os.listdir(hangDir + dateDir + "/" + logDir)
            fileIndex = findInList(logFileList, "flipvalue")
            if fileIndex != -1:
                # Reads data from file and appends new object to the result list
                logFile = open(hangDir + dateDir + "/" + logDir + "/" + logFileList[fileIndex], "r").readlines()
                hangList.append(readValuesFromLog(logFile))
    return hangList

def readValuesFromLog(logFile):
    """
    Returns an Injection object containig the values read from the file
    """
    # Initializes variables
    varName = ""
    varType = ""
    injectionLine = ""
    memContentBefore = ""
    newValue = ""
    faultModel = ""
    filename = ""
    funcName = ""
    # Reads every line in the file
    for line in logFile:
        # Extracts data from each line
        if re.search("^Fault Model:.*", line):
            # Fault Model: Random bit-flip
            faultModel = re.search("(?<=^Fault Model:).*", line).group().lstrip()

        elif re.search("^#0.+:\d+", line):
            # Injection line and file name
            # #0  0x0000000000400ff0 in updatePositions (elapsedTime_uSec=100000) at collisionDetection.c:106
            filename = re.search("(?<=\) at )[a-zA-Z0-9_\.]+", line).group().lstrip()
            injectionLine = re.search("(?<=:)[0-9]+", line).group().lstrip()

        elif re.search("^Memory content before bitflip:.*", line):
            # Memory content before bitflip:0x9a0x990x990x990x990x990xb90x3f
            memContentBefore = re.search("(?<=Memory content before bitflip:)[0-9xa-fA-F]+", line).group().lstrip()

        elif re.search("^frame name:.*", line):
            # Function name
            # frame name: updatePositions
            funcName = re.search("(?<=^frame name:).*", line).group().lstrip()

        elif re.search("^symbol name:.*", line):
            # symbol name: elapsedTime_Sec
            varName = re.search("(?<=^symbol name:).*", line).group().lstrip()

        elif re.search("^value:.*", line):
            # Value after bitflip
            # value: 1.3269862909504584e+89
            newValue = re.search("(?<=^value:).*", line).group().lstrip()

        elif re.search("^Type:.*", line):
            # Variable type
            # Type: A floating point type.
            varType = re.search("(?<=^Type:).*", line).group().lstrip()

    injectionBuf = Injection(varName, varType, injectionLine, memContentBefore, newValue, faultModel, filename, funcName)
    return injectionBuf

def findInList(listVar, content):
    """
    Returns index to searched element, or -1 if not found
    """

    for index, element in enumerate(listVar):
        if isinstance(element, str):
            if element.find(content) != -1:
                return index

    return -1

def printList(listVar):
    """
    Prints all the elements in a list
    """

    for element in listVar:
        print(element)

if __name__ == "__main__":
    main()
