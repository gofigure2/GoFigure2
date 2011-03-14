#import os
#address = os.getcwd()
#address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
#print address
<<<<<<< HEAD
#setBundlePath(address)
=======
setBundlePath(address)
>>>>>>> develop

start = time.time()

#Need to get to a new book mark spot in the easiest way to add a new one.


Tests = ("Cancel","No Name","Add New","Existing Name","Bookmark exists")
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
		type(Key.TAB+Key.TAB+Key.TAB)
		type("Test Description")
		click("ok.png")
		#assert that bookmark is in dropdown menu
		#assert that bookmark is in database

	if i == "Existing Name":
		wait(1)
		type("Test Bookmark")
		#first tab goes to cancel, then ok then description box
		type(Key.TAB+Key.TAB+Key.TAB)
		type("Test Description")
		click("ok.png")
		#assert popup message exists
		click("ok.png")
		click("cancel.png")

	if i == "Bookmark exists":
#To test that this bookmark already exists.

		click("Test_Bookmark.png")
		type("This book mark already exists")
		click("ok.png")
		#assert popup message exists
		click("ok.png")
		#need to select an existing bookmark and then add new book mark with different name



#To test bookmark description shows up
click("Bookmarks.png")
click("Open_a_bookmark.png")
wait(2)
hover("Test_Bookmark.png")
wait(5)

print "I'm waiting to find the description below"
#x = Region(4,990,200,10).find("Bookmark_Description.png")
assert exists("Bookmark_Description.png")
print "I found it"




#Test bookmark deletion

#Delete + cancel
click("File.png")
click("Bookmarks.png")
click("Delete_a_bookmark.png")
Region(500,282,283,271).click("Test_Bookmark.png")
click("cancel.png")


#Delete + ok
#click("File.png")
wait(1)
click("Bookmarks.png")
click("Delete_a_bookmark.png")
Region(500,282,283,271).click("Test_Bookmark.png")
click("ok.png")
click("yes.png")

#finally need to click on a different bookmark to see that quadview changes


end = time.time()
elapsedSec = end - start
print elapsedSec, "sec"