#will have to click trace and table to get right size for xyview images and navigation widget to see if things moved or not
#import os
#address = os.getcwd()
#address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"

#setBundlePath(address)
Toolbar = ("default","ZoomIcon.png","PanIcon.png","ActorPickingIcon.png")


Mouse = ("Left","Right","Middle","Scroll")
#Actions = ("single_click","double_click","drag_up2down","drag_down2up","drag_left2right","drag_right2left")

ViewRegion = ("XY_ViewRegion.png","XZ_ViewRegion.png","YZ_ViewRegion.png","XYZ_ViewRegion.png")
Init_view = [None]*4
pointx = [None]*4
pointy = [None]*4
for i in range(4):
	Init_view[i] = find(Pattern(ViewRegion[i]).similar(0.3))
	pointx[i] = Init_view[i].x+ int(round(0.3*(Init_view[i].w)))
	pointy[i] = Init_view[i].y+ int(round(0.6*(Init_view[i].h)))
	print pointx[i],pointy[i], "this is pointx and pointy"
	print Init_view[i]
	#Init_XZview = find(Pattern(ViewRegion[1]).similar(0.30))
	#Init_YZview = find(Pattern(ViewRegion[2]).similar(0.30))
	#Init_XYZview = find(Pattern(ViewRegion[3]).similar(0.30))


for mode in Toolbar:

	if mode != "default":
	 	click(mode)

	for j in range(4):
		#pointx= 2*(Init_view[j].x+Init_view[j].h)/3
		#pointy= 2*(Init_view[j].y+Init_view[j].w)/3
		#print pointx, pointy
		#single left click
		mouseMove(Location(pointx[j],pointy[j]))
		view_withmouse = capture(Init_view[j].getRect())
		click(Location(pointx[j],pointy[j]))
		#nothing should happen in all modes in all views
	
		assert exists(view_withmouse)

		#single right click
		rightClick(Location(pointx[j],pointy[j]))
		#nothing should happen in all modes in all views
		assert exists(Pattern(view_withmouse))

		#double right click
		
		mouseDown(Button.RIGHT)
		mouseUp(Button.RIGHT)
		mouseDown(Button.RIGHT)
		mouseUp(Button.RIGHT)
		#nothing should happen in all modes in all views
		assert exists(Pattern(view_withmouse))

		#single middle click
		
		mouseDown(Button.MIDDLE)
		mouseUp(Button.MIDDLE)
		#nothing should happen in all modes in all views
		assert exists(Pattern(view_withmouse))

		#double middle click
	
		mouseDown(Button.MIDDLE)
		mouseUp(Button.MIDDLE)
		mouseDown(Button.MIDDLE)
		mouseUp(Button.MIDDLE)
		#nothing should happen in all modes in all views
		assert exists(Pattern(view_withmouse))

		shift = 30
		mouseMove(Location(pointx[j]+shift,pointy[j]))
		view_withmouseRight = capture(Init_view[j].getRect())
		#left click drag left2right
		dragDrop(Location(pointx[j],pointy[j]),Location(pointx[j]+shift,pointy[j]))

		#should move figure to the right only in pan mode in all views
		#should rotate only xyz view to the right in all other modes.

		if mode == "PanIcon.png":
			#assert each view is shifted by 10 pixels
			rightShift = find(Pattern(view_withmouse))
			assert(rightShift.x == Init_view[j].x + shift)
			assert(rightShift.y == Init_view[j].y)
			assert(rightShift.h == Init_view[j].h)
			assert(rightShift.w == Init_view[j].w)
	
			
		else:	
			
			if j == 3:
			
				mouseMove(Location(pointx[0],pointy[0]))
				wait(3)
				#assert xyz view rotated to the right by 10 degrees
				assert exists(Pattern("1298594448537.png").similar(0.99))
				
				
			else:	
				
				assert exists(Pattern(view_withmouseRight))
				
	

	

		

	


	

		
	
	
     	


		