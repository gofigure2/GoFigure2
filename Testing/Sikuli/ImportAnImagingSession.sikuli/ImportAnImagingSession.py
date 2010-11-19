import os
address = os.getcwd()
#address = "home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

click("File.png")
click("Use_database.png")
click("Next.png")
#click("text_box.png")
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

click("Finish.png")
