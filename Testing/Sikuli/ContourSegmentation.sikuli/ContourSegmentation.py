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

ViewRegion = ("XY_ViewRegion.png","XZ_ViewRegion.png","YZ_ViewRegion.png","last")

for view in ViewRegion:
	
	
	if view == "last":
		for i in ViewRegion[0:3]:
			DrawContour(i)
	else:
		DrawContour(view)
	
	click("ContourSettings.png")
	#change line width from 3 to 7
	type(Key.UP+Key.UP+Key.UP+Key.UP+Key.UP)
	click("cancel.png")
	#check that nothing changed	


	click("ContourSettings.png")
	click("ok.png")
	#check that line width changed.

	click("ContourReinit.png")
	#check that line disapears 
	if view == "last":
		for i in ViewRegion[0:3]:
			DrawContour(i)
	else:
		DrawContour(view)
	
			
	click("Validate.png")
	#check that new contour is on View and in DB
	