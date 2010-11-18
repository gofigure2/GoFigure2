import os
address = os.getcwd()
#address = "home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

click("File.png")
click("Use_database.png")
click("Next.png")
click("Name_of_project.png".right(100))
