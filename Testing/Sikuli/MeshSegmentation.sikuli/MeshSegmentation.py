#import os
#address = os.getcwd()
address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
#print address
setBundlePath(address)

def MeshSegmentation(View):

	import random
	region = find(Pattern(View).similar(0.30))

	#could make it a straight line by moving differentials in one direction.
	xclick = random.randint(region.x,region.w+region.x)
	yclick = random.randint(region.y,region.h+region.y)

	click("Apply.png")



ViewRegion = ("XY_ViewRegion.png","XZ_ViewRegion.png","YZ_ViewRegion.png","last")

Filter = ("2DContour","3DSphere","3DLevelSet")


for shape in Filter:

	for view in ViewRegion:

		if view == "last":
			for i in ViewRegion[0:3]:
				MeshSegmentation(i)
		else:
			MeshSegmentation(view)