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

#NEED TO FIGURE OUT HOW TO SELECT 3DMeshExportTest.txt file for any user on any OS

click("File.png")
click("Close_all.png")

click("UseDatabaseIcon.png")
click("Next.png")
click("Next.png")
click("Finish.png")

click("NavigationIcon.png")
click("TableWidgetIcon.png")
click("MeshSegmentationIcon.png")




#compare before image in gofigureimage folder to after image taken here
#reg = find(Pattern("roughQuadViewRectangle.png").similar(0.30))
#this should find a rough 30% image and just get the coordinates and then take real image below
#table_quadview = reg.getRect()
#quadview_image_AFTER =capture(table_quadview)


f = open("quadview_image_BEFORE_address.txt","r")
quadview_BEFORE = f.read(quadview_image_BEFORE)

#make sure images are exact.
find(quadview_BEFORE).similar(.95)


#open GF
#import track
#close imaging session
#reopen imaging session
#compare screenshot of before with after.