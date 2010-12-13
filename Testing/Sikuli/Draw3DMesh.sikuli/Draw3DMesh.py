#import os
#address = os.getcwd()
address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
#print address
setBundlePath(address)

click("NavigationIcon.png")
click("TableWidgetIcon.png")
click("MeshSegmentationIcon.png")

import random
region = find(Pattern("XY_ViewRegion.png").similar(0.30))
print region
for i in [1,4]:
	#could make it a straight line by moving differentials in one direction.
	xclick = random.randint(region.x,region.w+region.x)
	yclick = random.randint(region.y,region.h+region.y)
	print xclick, yclick
	
	#change time point
	
	click(Location(xclick,yclick))
	click(find("Mode.png").right(200))
	click("Manual.png")
	click("Apply.png")