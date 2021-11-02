#!/usr/bin/python2

import argparse
import os.path
import re
import shutil

parser = argparse.ArgumentParser(description='Update session files of old API versions.\nIf the file already has the current API version, no changes are performed.\nOtherwise, the file is updated and the original version is copied to "[filename].orig"')
parser.add_argument('files', metavar='file', nargs='+', help='a file that should be updated')

args = parser.parse_args()

currentVersion = 20170511

for filename in args.files:
    print "Processing File \"" + filename + "\""

    if os.path.exists(filename):
        version = 0
        hasUpdated = False
        updatedText = ""
        file = open( filename, "r" )

        # first step: determine version
        line = file.readline()
        if re.match(r"###\s*FAnToM\s+Session\s*\Z", line) != None:
            line = file.readline()
            match = re.match(r"###\s*API\s+Version:\s+([0-9]+)\s*\Z", line)
            if match != None:
                version = int(match.group(1))
            else:
                file.seek(0)
        else:
            file.seek(0)

        # now scan through the file and fix API changes
        if version == currentVersion:
            print "\tThe file is already at the latest API version. Skipping..."
            file.close()
            continue
        # insert header
        updatedText += "### FAnToM Session\n"
        updatedText += "### API Version: " + str(currentVersion) + "\n"

        if version == 0:
            updatedText += "\n"

        if version < 20170511:
            print( "Warning: clipping plane definition has changed. Unfortunately, we need to reset the clipping planes. Please check and adapt." )

        for line in file:
            # skip comments
            if re.match("#", line):
                updatedText += line
                continue
            if version < 20140403:
                # camera
                line = re.sub(r'kernel\.setCamera\(([0-9]+), (.*)\)\s*\Z',
                              r'fantom.ui.setCamera(\1, fantom.ui.Camera(\2))\n',
                              line)
                # clipping plane
                line = re.sub(r'kernel\.setClippingPlane\(([0-9]+), "([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+([0-9.]+)\s+",\s*([01])\s*\)\s*\Z',
                              r'fantom.ui.setClippingPlane( fantom.ui.ClippingPlane(\1, fantom.math.Matrix4( (\2, \3, \4, \5, \6, \7, \8, \9, \10, \11, \12, \13, \14, \15, \16, \17) ), \18) )\n',
                              line)
                # color
                line = re.sub(r'kernel\.setColor\((.+)\)\s*\Z',
                              r'fantom.ui.setBackgroundColor( \1 )\n',
                              line)
                # rotation center
                line = re.sub(r'kernel\.setRotationCenter\((.+)\)\s*\Z',
                              r'fantom.ui.setRotationCenter( fantom.ui.RotationCenter( \1 ) )\n',
                              line)
                # make algorithm
                line = re.sub(r'kernel.makeAlgorithm\(', r'fantom.makeAlgorithm(', line)
                # algorithm position
                line = re.sub(r'kernel.move\(', r'fantom.ui.setAlgorithmPosition(', line)
                # runWithDependents
                line = re.sub(r'\.runTree\(', r'.runWithDependents(', line)
                # waitForAlgorithms
                line = re.sub(r'kernel.waitForSchedule\(', r'fantom.waitForAlgorithms(', line)
                # getCamera
                line = re.sub(r'kernel.getCamera\(', r'fantom.ui.getCamera(', line)
                # getAlgorithm
                line = re.sub(r'kernel.getAlgorithm\(', r'fantom.getAlgorithm(', line)
                # getAllAlgorithmNames
                line = re.sub(r'kernel.getAlgorithmList\(', r'fantom.getAllAlgorithmNames(', line)
                # getAvailableAlgorithms
                line = re.sub(r'kernel.getRegisteredAlgorithms\(', r'fantom.getAvailableAlgorithms(', line)
                # scheduleAllNecessary
                line = re.sub(r'kernel.scheduleAllNeeded\(', r'fantom.scheduleAllNecessary(', line)
                # takeSnapshot
                line = re.sub(r'kernel.takeSnapshot\(\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*(.+)\s*\)\s*\Z',
                              r'fantom.ui.takeSnapshot(\4, fantom.ui.ViewSize(\2, \3), \1, \5)\n',
                              line)
            if version < 20140515:
                # visibility of graphics outputs
                line = re.sub(r'\.setGraphicsVisibility\(',
                              r'.setVisualOutputVisible(',
                              line)
            if version < 20170511:
                # definition of clipping planes (unfortunately resetting all clipping planes)
                line = re.sub(r'(ClippingPlane\(.*)Matrix4\(\s*\([ ,0-9]+\)\s*\)',
                              r'\1Vector4(1,0,0,1)',
                              line)
            # add checks for future API changes here. guard them by an "if version < FutureAPIVersion:"

            updatedText += line

        file.close()

        shutil.move( filename, filename + ".orig" )
        file = open( filename, "w" )
        file.write(updatedText)
        file.close()
    else:
        print "\tThe file does not exist! Skipping..."
print "Finished processing files."
