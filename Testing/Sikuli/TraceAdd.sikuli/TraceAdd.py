import os
address = os.getcwd()
#address should be ~/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

start = time.time()


def AddNew(Region):
#	"""Clicks on add a new... in dropdown menu of TraceWidget for Color, CellType, SubCellType. """
	find(Region)
	a = getLastMatch()
	print a, "this is a"
	click(a.below(20))

#	click("add_a_new.png")


click("TraceWidgetIcon.png")

#three trace widget buttons
TraceObject = ("SelectedColor.png","SelectedCellType.png","SelectedSubCellType.png")
#four add a new... tests for each trace widget button
Action = ("Add New", "Cancel", "No Name", "Existing Name")

for a in Action:
		for i in TraceObject:
			AddNew(i)
			if i == ("SelectedColor.png") :
				click("Add_new_color.png")
			#chooses a color first, just for this button
				click("colortable.png")
				click("ok.png")
			elif i != ("SelectedColor.png") :
				if a == "Add New":
					click("Add_a_new_BLUE.png")
				if i == ("SelectedCellType.png"):
					click("Add_a_new_celltype.png") 
				else:
					click("Add_a_new_subcelltype.png")	
				
			if a == "Add New":
				wait(1)
				type("Widget Test")
				click("ok.png")
				assert exists("previousStateWidgetTest.png")

				#need assert new name and color comes up in mysql
			if a == "Cancel":
				click("cancel.png")
				assert exists("previousStateWidgetTest.png")
				
			if a == "No Name":
				click("ok.png")
				wait(1)
				
				assert exists("errorPopUp.png")
				click("ok.png")
				click("cancel.png")
				assert exists("previousStateWidgetTest.png")
				
			if a == "Existing Name":
				wait(1)
				type("Widget Test")
				click("ok.png")
				
				assert exists("errorPopUp.png")
				click("ok.png")
				click("cancel.png")
				assert exists("previousStateWidgetTest.png")


end = time.time()
elapsedSec = end - start
print elapsedSec, "sec"
