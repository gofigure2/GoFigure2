import os
address = os.getcwd()
#address should be ~/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

start = time.time()

def ClickDelete(Region):
#	"""Clicks on add a new... in dropdown menu of TraceWidget for Color, CellType, SubCellType. """
	find(Region)
	a = getLastMatch()
	print a, "this is a"
	click(a.below(20))

	click("delete.png")
	wait(1)

def ChooseDeleteWidgetTest(Region):
	find(Region)
	a = getLastMatch()
	a.inside().click("WidgetTestDelete.png")
	
	
TraceObject = ("SelectedColor.png","SelectedCellType.png","SelectedSubCellType.png")
Action = ("Cancel","DeleteFields")
for a in Action:
	for i in TraceObject:
		ClickDelete(i)
		
		if a == "Cancel":
			
			click("cancel.png")
			#assert that same state is there.
		
		if a == "DeleteFields":
			
			onChange(getCenter().click("WidgetTestDelete.png"))
					
			click("ok.png")
			click("yes.png")
			#assert that previous state is there

end = time.time()
elapsedSec = start - end
print elapsedSec, "sec"
			