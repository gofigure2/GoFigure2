#will have to click trace and table to get right size for xyview images and navigation widget to see if things moved or not

Toolbar = ("default","ZoomIcon.png","PanIcon.png","ActorPickingIcon.png")


Mouse = ("Left","Right","Middle","Scroll")
Actions = ("single_click","double_click","drag_up2down","drag_down2up","drag_left2right","drag_right2left")

ViewRegion = ("XY_ViewRegion.png","XZ_ViewRegion.png","YZ_ViewRegion.png","XYZ_ViewRegion.png")

for i in [0..3]:
	Init_view[i] = find(Pattern(ViewRegion[i]).similar(0.30))
	
	#Init_XZview = find(Pattern(ViewRegion[1]).similar(0.30))
	#Init_YZview = find(Pattern(ViewRegion[2]).similar(0.30))
	#Init_XYZview = find(Pattern(ViewRegion[3]).similar(0.30))


for mode in Toolbar:

	if mode != "default":
	 	click(mode)

	for j in [0..3]:
		point= [2*(Init_view[j].x+Init_view[j].h)/3, 2*(Init_view[j].y+Init_view[j].w)/3]

		#single left click
		click(Location(point))
		#nothing should happen in all modes in all views
		assert exists(Init_view[j])

		#single right click
		rightClick(Location(point))
		#nothing should happen in all modes in all views
		assert exists(Init_view[j])

		

				
	


		

	

	

		

	


	

		
	
	
     	


		