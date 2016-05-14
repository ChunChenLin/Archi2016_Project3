import os
import sys
import commands


projDict = { "1":"single_cycle",
                "2":"pipeline",
                "3":"CMP"}
projNum = ["proj=1", "proj=2", "proj=3"]
compFlag = ["-c", "-nc"]

# print sys.argv


if len(sys.argv) == 3 and (sys.argv[1] in projNum) and (sys.argv[2] in compFlag):


        # handler main process
        projNum = sys.argv[1].split('=')[1];
        projName = projDict[projNum]

        # Check current tar.gz do not exist previous file
        if os.path.exists("{}.tar.gz".format(projName)) :
            print "WATCH OUT! {}.tar.gz already exists!".format(projName)
            print "Script halt!"
            exit()

        # Check Project Folder
        if os.path.exists(projName) and os.path.isdir(projName):
            print "PASS, {} project folder exists here".format(projName);
        else:
            print "ERROR, {} project folder does not exist here".format(projName);

        # Check Project Folder Containing simulator & testcase folder
        # Simulator
        simuDir = projName+"/simulator"
        if os.path.exists(simuDir) and os.path.isdir(simuDir):
            print "PASS, simulator folder exists in {}".format(simuDir);
        else:
            print "ERROR,simulator folder does not exist in {}".format(simuDir);

        # TestCase
        tcDir = projName+"/testcase"
        if os.path.exists(tcDir) and os.path.isdir(tcDir):
            print "PASS, testcase folder exists in {}".format(tcDir);
        else:
            print "ERROR,testcase folder does not exist in {}".format(tcDir);

        # Simulator Make Test
        os.chdir("{}/simulator".format(projName))
        retv = os.system('make > /dev/null 2>&1')
        if retv == 0:
            print('PASS => '+'Successful to do "make"')
        else:
            print('ERROR => '+'Fail to do "make"')
            exit()

        # Check Executable Exist
        if os.path.exists("{}".format(projName)):
            print 'PASS => program "/simulator/{}\"" exists'.format(projName)
        else:
            print 'ERROR => program "/simulator/{}\"" doesn\'t exists'.format(projName)

        # Check Image File Exist
        os.chdir("../testcase".format(projName))
        if os.path.exists('iimage.bin'):
            print('PASS => "testcase/iimage.bin\"" exists')
        else:
            print('ERROR => "testcase/iimage.bin\"" doesn\'t exists')

        if os.path.exists('dimage.bin'):
            print('PASS => "testcase/dimage.bin\"" exists')
        else:
            print('ERROR => "testcase/dimage.bin\"" doesn\'t exists')

        os.chdir("../..")
        if sys.argv[2] == "-c":
            retv = os.system('tar -zcf {}.tar.gz {}> /dev/null 2>&1'.format(projName,projName))
            if retv == 0 and os.path.exists("{}.tar.gz".format(projName)) :
                print('OK => '+'Successful to do "tar"')
            else:
                print('ERROR => '+'Fail to do "tar"')

elif len(sys.argv) == 2:
    if sys.argv[1] == '-h':
        # helps content
        # print "## {} manual ##".format(sys.argv[0])
        print ""
        print "FORMAT : python {} [proj=num] [-c/-nc]".format(sys.argv[0])
        print ""
        print "ENVIRONMENT: linux ubuntu "
        print ""
        print "EXAMPLE:"
        print "        EX: python {} proj=1 -c".format(sys.argv[0])
        print "LOCATION:"
        print "      project folder to be check should be under same directory"
        print "        EX: - dir"
        print "              - pipeline(to be check)"
        print "              - {}".format(sys.argv[0])
        print "PARAMETER:"
        print "     proj=num (Select Project Number): , num = 1,2,3 "
        print "            proj=num represent which project format you are checking"
        print ""
        print "     -c/-nc (Compress Flag):, -c: compress your folder, -nc: not compress"
        print "            After executing script, automatically compress folder or not"

    else:
        print "Please type 'python {} -h' to find parameter definition".format(sys.argv[0])
else:
    print "Please type 'python {} -h' to find parameter definition".format(sys.argv[0])