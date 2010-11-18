import os
address = os.getcwd()
#address should be ~/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images

#address = address+"/gofigure2_images"
print address
setBundlePath(address)


click("Settings.png")
click("Database_arrow.png")
click("Set_up_database.png")
wait(1)
type("gofigure") 
Key.TAB 
type("gofigure")
click("Next_arrow.png")
click("ok.png")
Key.BACKSPACE
click("ok.png")
click("ok.png")
Key.TAB+Key.TAB
type("First")
Key.TAB+Key.TAB
type("Last")
Key.TAB+Key.ENTER
click("ok.png")
click("Next_arrow.png")
wait(1)
type("microscope")
Key.TAB+Key.ENTER
