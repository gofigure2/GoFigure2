import os
address = os.getcwd()
#address = "/home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

click("File.png")
click("Use_database.png")
click("Next.png")
#click("text_box.png")
#click on create new project
wait(1)
click("Next.png")
assert exists("Please_enter_name_of_project.png")
click("ok.png")

click("Add_author.png")
type("Second"+Key.TAB+Key.TAB+"Author")
click("Create_Author.png")
#user cannot press enter for Create Author button 
click("ok.png")
#BUG: the create author popup does not disappear after you create one author. Need to x out
click("blue_x.png")


keyDown(Key.SHIFT)
type(Key.TAB+Key.TAB+Key.TAB+Key.TAB)
keyUp(Key.SHIFT)
wait(1)
type("Project Name")
click("Next.png")
keyDown(Key.SHIFT)
type(Key.TAB+Key.TAB+Key.TAB+Key.TAB+Key.TAB)
keyUp(Key.SHIFT)
wait(1)
type("ImagingSession1")
type(Key.TAB+Key.TAB)

length = len(address) -50
user = address[0:length]


#this info should be input to function
type(user+"/Data/t00-09/image-PL00-CO00-RO00-ZT00-YT00-XT00-TM0009-ch01-zs0070.png")
click("Add_microscope.png")
type("microscope2")
click("Create_microscope.png")
click("ok.png")
click("blue_x.png")




click("Finish.png")
