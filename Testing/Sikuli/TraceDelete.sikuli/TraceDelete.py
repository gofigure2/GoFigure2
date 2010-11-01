import os
address = os.getcwd()
#address should be ~/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

start = time.time()

def Delete(Region):
#	"""Clicks on add a new... in dropdown menu of TraceWidget for Color, CellType, SubCellType. """
	find(Region)
	a = getLastMatch()
	print a, "this is a"
	click(a.below(20))

	click("delete.png")
	
TraceObject = ("SelectedColor.png","SelectedCellType.png","SelectedSubCellType.png")
Action = ("DeleteFields","Cancel")
for a in action:
	for i in TraceObject:
		Delete(i)

		if a = "DeleteFields":
			
		
	
			click("WidgetTestDelete.png")
			click("ok.png")
			click("yes.png")
			#assert that previous state is there

		if a = "Cancel":
			click("WidgetTestDelete.png")
			click("cancel.png")
			#assert that same state is there.