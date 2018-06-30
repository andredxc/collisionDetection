#!/bin/python
import re, os, getopt, sys, pickle

class Injection:
    """
    Stores information about a single fault injection
    """
    def __init__(self, varName, varType, line, memContentBefore, newValue, faultModel, fileName, functionName, faultResult, sdcCriticality):
        self.varName = varName
        self.varType = varType
        self.line = line
        self.memContentBefore = memContentBefore
        self.newValue = newValue
        self.faultModel = faultModel
        self.fileName = fileName
        self.functionName = functionName
        self.faultResult = faultResult
        self.sdcCriticality = sdcCriticality # -1 = not evaluated, 0 = not critical, >0 = how critical

    def __repr__(self):

        if self.varType.find("structure") > -1 or self.varType.find("array") > -1:
            # Doesn't print values for arrays or structs
            return "faultResult: %s\nvarName: %s\nvarType: %s\nline: %s\nfaultModel: %s\nfileName: %s\nfunctionName: %s\n" %(self.faultResult, self.varName, self.varType, self.line, self.faultModel, self.fileName, self.functionName)
        else:
            return "faultResult: %s\nvarName: %s\nvarType: %s\nline: %s\nmemCont: %s\nnewValue: %s\nfaultModel: %s\nfileName: %s\nfunctionName: %s\n" %(self.faultResult, self.varName, self.varType, self.line, self.memContentBefore, self.newValue, self.faultModel, self.fileName, self.functionName)

class Result:
    """
    Stores information about all results from multiple fault injections
    """
    def __init__(self, failedList, hangList, maskedList, sdcList):
        self.failedList = failedList
        self.hangList = hangList
        self.maskedList = maskedList
        self.sdcList = sdcList

    def __repr__(self):

        return "--Result--\n\tFailed: %d\n\tHangs: %d\n\tMasked: %d\n\tSDCs: %d\n" %(len(self.failedList), len(self.hangList), len(self.maskedList), len(self.sdcList))

#-------------------------------------------------------------------------------
def main(argv):

    logDir = "/home/andre/ufrgs/ftf/initial_logs/collisionDetection-random/"
    newLogs = False

    # Reading arguments from command line
    try:
        opts, args = getopt.getopt(argv, "", ["help", "new"])
        for opt, arg in opts:
            if opt == "--new":
                newLogs = True
            elif opt == "--help":
                print("Usage:")
                print("\t--new to read all log files from the directory listed in \'logDir\' variable.")
                print("\tNo argument to read values from previously written \'result.pkl\' file.")
                sys.exit(1);
    except getopt.GetoptError:
        print("Problem with provided arguments")
        sys.exit(0)

    if(newLogs):
        # Initialization
        failedList = list()
        hangList = list()
        maskedList = list()
        noOutputList = list()
        sdcList = list()

        # Searches all directories
        for fileName in os.listdir(logDir):
            # Skip regular files
            if not os.path.isdir(logDir + fileName):
                continue

            if fileName == "failed-injection":
                # Failed
                print("Reading failed injection logs...")
                failedList = handleLogDir(failedList, logDir+"failed-injection", "failed")

            elif fileName == "hangs":
                # Hangs
                print("Reading hang injections logs...")
                hangList = handleLogDir(hangList, logDir+"hangs/", "hang")

            elif fileName == "masked":
                # Masked
                print("Reading masked injections logs...")
                maskedList = handleLogDir(maskedList, logDir+"masked/", "masked")

            elif fileName == "noOutputGenerated":
                # No output
                print("Reading noOutput injections logs...")
                noOutputList = handleLogDir(noOutputList, logDir+"noOutputGenerated/", "noOutputGenerated")

            elif fileName == "sdcs":
                # SDCs
                print("Reading SDC injections logs...")
                sdcList = handleLogDir(sdcList, logDir+"sdcs/", "sdc")

        # Create Result object
        result = Result(failedList, hangList, maskedList, sdcList)
        # Save variables with pickle
        pickle.dump(result, open("result.pkl", "wb"))

    else:
        # Reads variables with pickle
        try:
            result = pickle.load(open("result.pkl", "rb"))
        except:
            print("Couldn't read pickle values!")
            sys.exit(0)

    # Print number of elements in each list
    print(result)
    # listSdcCriticality(result)
    listCriticalVariables(result)

#-------------------------------------------------------------------------------
def listSdcCriticality(result):
    """
    List SDC cricitality
    """
    sdcCriticalityFilePath = "/home/andre/ufrgs/ftf/sdcCriticality.log"
    listBuf = result.sdcList
    listBuf = sorted(listBuf, key=lambda x: x.sdcCriticality)
    listBuf.reverse()

    # Log file
    logFile = open(sdcCriticalityFilePath, "w")

    totalElements = 2819.0

    for percentageCount in range(2,101,2):
        elementCount = 0
        for item in listBuf:
            if (item.sdcCriticality > percentageCount-2) and (item.sdcCriticality <= percentageCount):
                elementCount += 1


        logFile.write("%d: %.2f\n" %(percentageCount, float(elementCount)/totalElements*100.0))
    logFile.close()

def listCriticalVariables(result):
    """
    Lists number of ocurrances of each variable in each Result list
    Outputs results into listCriticalVariables.dat file
    """

    sdcCritVarFilePath = "/home/andre/ufrgs/ftf/sdcCriticalVariables.log"
    hangCritVarFilePath = "/home/andre/ufrgs/ftf/hangCriticalVariables.log"
    sdcVarDic = dict()
    hangsVarDic = dict()

    # Creates dicts for variable ocurrances
    for inj in result.hangList:
        # Counts the number of times each variable causes an SDC
        if not (inj.varName in hangsVarDic.keys()):
            # Ads variable to dictionary
            hangsVarDic[inj.varName] = 1
        else:
            # Increments variable occurrance
            hangsVarDic[inj.varName] += 1

    hangsVarDic = sorted(hangsVarDic.items(), key=lambda x: x[1])

    for inj in result.sdcList:
        # Counts the number of times each variable causes an SDC
        if not (inj.varName in sdcVarDic.keys()):
            # Ads variable to dictionary
            sdcVarDic[inj.varName] = 1
        else:
            # Increments variable occurrance
            sdcVarDic[inj.varName] += 1

    sdcVarDic = sorted(sdcVarDic.items(), key=lambda x: x[1])

    #Print lists into log file
    hangLogFile = open(hangCritVarFilePath, "w")
    sdcLogFile = open(sdcCritVarFilePath, "w")

    for item in hangsVarDic:
        hangLogFile.write("%s: %s\n" %(item[0], item[1]))

    for item in sdcVarDic:
        sdcLogFile.write("%s: %s\n" %(item[0], item[1]))

    sdcLogFile.close()
    hangLogFile.close()

#-------------------------------------------------------------------------------
def sdcCriticality(dirPath):
    """
    Compares the number data lines between gold and corrupted outputs
    Return values: -1 = Error, 0 = Not critical, >0 = How critical

    gold file has a total of 222082 lines, 294 of which have no data in them.
    The total number of useful lines is 221788
    """

    faultyFileLines = 0
    faultyFileName = "outputFile"
    goldFileLines = 221788

    faultyContent = open(dirPath+"/"+faultyFileName, "r").readlines()
    count = 0
    for line in faultyContent:
        # Counts the number of useful lines
        if re.search("New Iteration*", line):
            count += 1

    faultyFileLines = len(faultyContent) - count
    absDifference = abs(goldFileLines - faultyFileLines)
    # Percentage difference
    print("Calculating: %d/%d" %(absDifference, goldFileLines))
    perDifference = (float(absDifference)/float(goldFileLines)) * 100.0
    print("Percentage difference: %f" %(perDifference))
    return(perDifference)

def handleLogDir(injList, hangDir, faultResult):

    for dateDir in os.listdir(hangDir):
        # Every date directory
        if not os.path.isdir(hangDir+dateDir):
            # Skip regular files
            continue
        for logDir in os.listdir(hangDir+dateDir):
            # Every injection directory
            # Finds the useful log file
            logFileList = os.listdir(hangDir + dateDir + "/" + logDir)
            fileIndex = findInList(logFileList, "flipvalue")
            if fileIndex != -1:
                # Reads data from file and appends new object to the result list
                logFile = open(hangDir + dateDir + "/" + logDir + "/" + logFileList[fileIndex], "r").readlines()
                injBuffer = readValuesFromLog(logFile, faultResult)
                # Performs application specific SDC criticality analysis
                if faultResult == "sdc":
                    injBuffer.sdcCriticality = sdcCriticality(hangDir + dateDir + "/" + logDir)

                injList.append(injBuffer)

    return injList

def readValuesFromLog(fileContent, faultResult):
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
    for line in fileContent:
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

    injectionBuf = Injection(varName, varType, injectionLine, memContentBefore, newValue, faultModel, filename, funcName, faultResult, -1)
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
    main(sys.argv[1:])
