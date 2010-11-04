import os
address = os.getcwd()
#address should be ~/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

start = time.time()

#Need to get to a new book mark spot in the easiest way to add a new one.


Tests = ("Cancel","No Name","Add New","Existing Name")
for i in Tests:
	
	click("Bookmarks.png")
	click("Add_a_bookmark.png")
	
	if i == "Cancel":
		click("cancel.png")
		#assert that goes back to same bookmark state
		
	if i == "No Name":
		click("ok.png")
		#assert popup message exists
		click("ok.png")
		click("cancel.png")

	if i == "Add New":
		wait(1)
		type("Test Bookmark")
		Key.TAB
		type("Test Description")
		click("ok.png")
		#assert that bookmark is in dropdown menu
		#assert that bookmark is in database

	if i == "Existing Name":
		wait(1)
		type("Test Bookmark")
		#first tab goes to cancel, then ok then description box
		Key.TAB
		Key.TAB
		Key.TAB
		type("Test Description")
		click("ok.png")
		#assert popup message exists
		click("ok.png")
		click("cancel.png")



#To test that this bookmark already exists.		
click("Bookmarks.png")
click("Open_a_bookmark.png")
click("Test_Bookmark.png")
#assert popup message exists
click("ok.png")		
		#need to select an existing bookmark and then add new book mark with different name


#To test deletion of bookmark
click("Delete_a_bookmark.png")
click("Open_a_bookmark.png")
hover("Test_Bookmark.png")
x = Region(4,990,200,10).find("Bookmark_Description.png")
assert exists(x)




end = time.time()
elapsedSec = end - start
print elapsedSec, "sec"