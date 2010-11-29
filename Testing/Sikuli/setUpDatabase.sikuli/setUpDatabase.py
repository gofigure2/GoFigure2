import os
address = os.getcwd()
#address = "home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

for create in [1,2]:
	click("Settings.png")
	hover("Database_arrow.png")
	click("Set_up_database.png")
	assert exists("Create_user_for_MySQL_DB.png")
	if create == 1:
		click("cancel.png")
		assert not exists("Create_user_for_MySQL_DB.png",0)
	if create == 2:
		click("Next_arrow.png")
		assert exists("Please_fill_all_fields.png")
		click("ok.png")
		assert not exists("Please_fill_all_fields.png",0)
		
		type(Key.TAB+Key.TAB)
		wait(2)
		type("gofigure" +Key.TAB + "gofigure")

		#click("Next_arrow.png")
		#assert exists("Root_Password.png")
		#type(Key.TAB+Key.ENTER)
		#assert exists("There_is_problem_existing_users.png")
		#click("ok.png")
			#gofigure crashes at this step
		#assert not exists("Root_Password.png")
	
		for i in [1,2]:
			click("Next_arrow.png")
			assert exists("Root_Password.png")
			wait(1)
			type(Key.BACKSPACE)#+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE)
			click("ok.png")
			assert exists("create_database_for_this_user.png")
			if i == 1:
				type(Key.ENTER)
				assert not exists("create_database_for_this_user.png")
			if i == 2:
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
