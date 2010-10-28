
setBundlePath("/home/es1076/SVNROOT/evan/sikuli/TraceAdd.sikuli")

start = time.time()

def AddNew(Region):
#	""" TraceWidget for Color, CellType, SubCellType. """
	find(Region)
	a = getLastMatch()
	print a, "this is a"
	click(a.below(20))

	click("1287656896225.png")

#x = find("Cell Contour")
#print x
TraceObject = ("1285599876548.png","1287655664410.png","1287655672392.png")
Action = ("Add New", "Cancel", "No Name", "Existing Name")
#test 1: Add a new...
for a in Action:
		for i in TraceObject:
			AddNew(i)
			if i == ("1285599876548.png") :
				click("1288125548622.png")
				click("1285598278725.png")
			if a == "Add New":
				wait(1)
				type("Widget Test")
				click("1285598344252.png")
				#assert new name comes up on widget with correct color
				#assert new name and color comes up in mysql
			if a == "Cancel":
				click("1287654735420.png")
				#assert previous state is found
			if a == "No Name":
				click("1285598344252.png")
				wait(1)
				#cannot use assertExist in sikuli editor
				assertExist("1287741083818.png")
				click("1285598344252.png")
				click("1288125769063.png")
			if a == "Existing Name":
				type("Widget Test")
				click("1285598344252.png")
				#cannot use assertExist in sikuli editor
				assertExist("1287741123199.png")
				click("1285598344252.png")
				click("1288125769063.png")
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
