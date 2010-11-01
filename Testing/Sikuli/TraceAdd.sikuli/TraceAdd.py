import os
address = os.getcwd()
#address should be ~/GITROOT/GoFigure2/Testing/Sikuli

address = address+"/gofigure2_images"
print address
setBundlePath(address)

start = time.time()

def AddNew(Region):
#	""" TraceWidget for Color, CellType, SubCellType. """
	find(Region)
	a = getLastMatch()
	print a, "this is a"
	click(a.below(20))

	click("add_a_new.png")

#x = find("Cell Contour")
#print x
TraceObject = ("SelectedColor.png","SelectedCellType.png","SelectedSubCellType.png")
Action = ("Add New", "Cancel", "No Name", "Existing Name")
#test 1: Add a new...
for a in Action:
		for i in TraceObject:
			AddNew(i)
			if i == ("SelectedColor.png") :
				click("colortable.png")
				click("ok.png")
			if a == "Add New":
				wait(1)
				type("Widget Test")
				click("ok.png")
				#assert new name comes up on widget with correct color
				#assert new name and color comes up in mysql
			if a == "Cancel":
				click("cancel.png")
				
			if a == "No Name":
				click("ok.png)
				wait(1)
				
				onAppear("errorPopUp.png")
				click("ok.png")
				click("cancel.png")
				assert exists()
			if a == "Existing Name":
				type("Widget Test")
				click("ok.png")
				#cannot use assertExist in sikuli editor
				onAppear("errorPopUp.png")
				click("ok.png")
				click("cancel.png")
#call time elapse function

#test 2: Add a new... and click cancel

#for i in tests:
#	AddNew(i)
#		if i == ("1285599876548.png") :
#			click("1287653361098.png")
#			click("1285598278725.png")

#	click("1287654735420.png")
	#assertExists( previous state )
#test 3: Add a new... with no name entered
#click("1285598344252.png")

#test 4: Add a new... with same name entered

end = time.time()
elapsedSec = end - start
print elapsedSec, "sec"
