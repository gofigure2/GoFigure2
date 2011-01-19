import os
address = os.getcwd()
#address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
#print address
setBundlePath(address)

click("ContourSegmentationIcon.png")
click("TableWidgetIcon.png")
wait(1)

xyView = find(Pattern("XY_ViewRegion.png").similar(0.10))
print xyView.x, xyView.y, xyView.h

startx = xyView.x + 100
starty = xyView.y + xyView.h - 100
click(Location(startx,starty))
print startx, starty
wait(1)
Secondx = startx+10
Secondy = starty+10
click(Location(Secondx,Secondy))
wait(1)
Thirdx = Secondx+10
Thirdy = Secondy -10
click(Location(Thirdx,Thirdy))
Forthx = Thirdx - 10
Forthy = Thirdy - 10
wait(1)
click(Location(Forthx,Forthy))
wait(1)

click(Location(startx,starty))
wait(1)

#click("Validate.png")