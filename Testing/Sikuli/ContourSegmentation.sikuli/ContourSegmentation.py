import os
address = os.getcwd()
#address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
#print address
setBundlePath(address)

def DrawContour(View):
	
	xyView = find(Pattern(View).similar(0.30))
	print xyView.x, xyView.y, xyView.h

	startx = xyView.x + int(round((0.3)*xyView.w))
	starty = xyView.y + int(round((0.6)*xyView.h))
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


click("ContourSegmentationIcon.png")
click("TableWidgetIcon.png")
wait(1)

ViewRegion = ("XY_ViewRegion.png","XZ_ViewRegion.png","YZ_ViewRegion.png")

for view in ViewRegion:

	DrawContour(view)

	#click("Validate.png")