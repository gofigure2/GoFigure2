import os
address = os.getcwd()
#address = "home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
print address
setBundlePath(address)


click("Settings.png")
hover("Database_arrow.png")
click("Set_up_database.png")
wait(1)
type("gofigure" +Key.TAB + "gofigure")
click("Next_arrow.png")
wait(1)
type(Key.BACKSPACE)#+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE)
click("ok.png")
click("ok.png")
wait(3)
type("First"+Key.TAB+Key.TAB+"Last"+Key.TAB+Key.ENTER)
click("ok.png")
click("Next_arrow.png")
wait(1)
type("microscope"+Key.TAB+Key.ENTER)
click("ok.png")
click("Finish.png")
click("ok.png")
