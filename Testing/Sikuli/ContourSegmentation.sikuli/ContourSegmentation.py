#import os
#address = os.getcwd()
#address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
#print address
#setBundlePath(address)

def DrawContour(View,xdecimal,ydecimal):
	
	xyView = find(Pattern(View).similar(0.30))
	print xyView.x, xyView.y, xyView.h

	startx = xyView.x + int(round((xdecimal)*xyView.w))
	starty = xyView.y + int(round((ydecimal)*xyView.h))
	click(Location(startx,starty))
	
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

click("TableWidgetIcon.png")
click("ContourSegmentationIcon.png")

wait(1)

ViewRegion = ("XY_ViewRegion.png","XZ_ViewRegion.png","YZ_ViewRegion.png","last")

for view in ViewRegion:
		
	if view == "last":
		for i in ViewRegion[0:2]:
			DrawContour(i,0.5,0.5)
	else:
		DrawContour(view,0.3,0.6)
	
	
	#reg = find(Pattern(view).similar(0.30))

	#table_quadview = reg.getRect()

	#quadview_image_BEFORE =capture(table_quadview)
	
	
	click("ContourSettings.png")
	#change line width from 3 to 7
	type(Key.UP+Key.UP+Key.UP+Key.UP+Key.UP)
	click("cancel.png")

	#find(Pattern(quadview_image_BEFORE).similar(.95))

	#check that nothing changed	
	#reg = find(Pattern(view).similar(0.30))
	#this should find a rough 30% image and just get the coordinates and then take real image below
	#table_quadview = reg.getRect()

	#quadview_image_BEFORE =capture(table_quadview)

#can find 1 instance of blue circle image and then 3 instances of it.  Also red circles for validation.


	click("ContourSettings.png")
	click("ok.png")
	#check that line width changed.
	
	click("ContourReinit.png")
	
	click("ContourSettings.png")
	#change line width from 3 to 7
	type(Key.UP+Key.DOWN+Key.DOWN+Key.DOWN+Key.DOWN)
	click("ok.png")
	#check that line disapears 
	if view == "last":
		for i in ViewRegion[0:2]:
			DrawContour(i,0.5,0.5)
	else:
		DrawContour(view,0.3,0.6)
	
			
	click("Validate.png")
	click("ContourReinit.png")
	#check that new contour is on View and in DB



click("Manual.png")
type(Key.DOWN+Key.ENTER)

import random

for channel in [0,2]:
	if channel == 2:
		keyDown(Key.SHIFT)
		type(Key.TAB)
		keyUp(Key.SHIFT)
		type(Key.DOWN)
		
	for view in ViewRegion:
	
		if view == "last":
			for i in ViewRegion[0:2]:
				region = find(Pattern(i).similar(0.30))
				xclick = random.randint(region.x,region.w+region.x)
				yclick = random.randint(region.y,region.h+region.y)
				click(Location(xclick,yclick))
				click("Apply.png")
		else:
			region = find(Pattern(view).similar(0.30))
			xclick = random.randint(region.x,region.w+region.x)
			yclick = random.randint(region.y,region.h+region.y)
			click(Location(xclick,yclick))
			click("Apply.png")