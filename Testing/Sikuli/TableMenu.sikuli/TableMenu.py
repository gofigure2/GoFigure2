import os
address = os.getcwd()
#address = home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

start = time.time()

click("MeshSegmentationIcon.png")
click("TableWidgetIcon.png")
wait(2)
#tab = ("Table_ContourTab.png","Table_MeshTab.png","Table_TraceTab.png")


tab = find("Table_MeshTab.png")
hover(tab)
from java.awt import Robot
robot = Robot()
robot.mouseWheel(-1)
tab = find("Table_ContourTab4.png")


for i in [0,1]:
	if i == 1:
		click(tab.right(85))

	rightClick(tab.right(300))
	wait(1)
	type(Key.DOWN+Key.DOWN+Key.DOWN+Key.ENTER)
	type(Key.DOWN+Key.DOWN+Key.DOWN+Key.ENTER)
	#gofigure crashed once

	if i == 0:
		assert exists("Please_select_at_least_one.png")
		click("ok.png")

	#hover(tab)
	#from java.awt import Robot
	#robot = Robot()
	#robot.mouseWheel(1)
	
hover(tab)
from java.awt import Robot
robot = Robot()
robot.mouseWheel(-1)


for i in [0,1]:
	if i == 1:
		click(tab.right(85))
	
	rightClick(tab.right(300))
	wait(1)
	type(Key.DOWN+Key.DOWN+Key.DOWN+Key.ENTER)
	type(Key.DOWN+Key.DOWN+Key.DOWN+Key.ENTER)
	#gofigure crashed once

	if i == 0:
		assert exists("Please_select_at_least_one.png")
		click("ok.png")

	hover(tab)
	from java.awt import Robot
	robot = Robot()
	robot.mouseWheel(1)	
