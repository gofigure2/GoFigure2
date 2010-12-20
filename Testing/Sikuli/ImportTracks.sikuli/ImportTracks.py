import os
address = os.getcwd()
#address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

#2 scripts: one before GF closes and one after creating database. 
#will need script to create meshes on 1 track in different time points.

click("Tools.png")
click("Import.png")
a = find("3DMeshes.png")
click(a.below(10))
wait(2)
type("3DMeshExportTest.txt")
type(Key.ENTER)
#NEED TO FIGURE OUT HOW TO SELECT 3DMeshExportTest.txt file for any user on any OS

#click("File.png")
#click("Close_all.png")
wait(2)
keyDown(Key.CTRL)
type("w")
keyUp(Key.CTRL)

click("UseDatabaseIcon.png")
click("Next.png")
click("Next.png")
click("Finish.png")
wait(5)

click("NavigationIcon.png")
click("TableWidgetIcon.png")
click("MeshSegmentationIcon.png")



hover("Table_MeshTab.png")

from java.awt import Robot
robot = Robot()
robot.mouseWheel(1)

#hover("Table_MeshTab.png")
#rightClick(region.getLastMatch().right(200).above(100))
rightClick("RedTrack1.png")
type(Key.DOWN+Key.ENTER+Key.DOWN+Key.DOWN+Key.DOWN+Key.ENTER)

keyDown(Key.CTRL)
type("d"+"d"+"d")
keyUp(Key.CTRL)
wait(6)

click("TableWidgetIcon.png")
click("WhiteArrowIcon.png")
click("TableWidgetIcon.png")
click("MeshSegmentationIcon.png")

#compare before image in gofigureimage folder to after image taken here
#reg = find(Pattern("roughQuadViewRectangle.png").similar(0.30))
#this should find a rough 30% image and just get the coordinates and then take real image below
#table_quadview = reg.getRect()
#quadview_image_AFTER =capture(table_quadview)


#f = open(address+"/quadview_image_BEFORE_address.txt","r")
#quadview_BEFORE = f.read()

#print quadview_BEFORE

#import shutil
#shutil.copy2(quadview_BEFORE,address)

#print "copied address"
#make sure images are exact.
quadview_image_AFTER = capture(264,157,852,792)
print "captured image"

import shutil
shutil.copy2(quadview_image_AFTER,address+"/quadview_image_AFTER.png")

print "copied image"

find("quadview_image_BEFORE.png").similar(.50)


#open GF
#import track
#close imaging session
#reopen imaging session
#compare screenshot of before with after.