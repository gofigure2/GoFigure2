import os
address = os.getcwd()
#address = "home/es1076/GITROOT/GoFigure2/Testing/Sikuli/gofigure2_images"

#address = address+"/gofigure2_images"
print address
setBundlePath(address)

click("UseDatabaseIcon.png")
click("Next.png")
assert exists("Have_not_set_up_GFDB.png")
click("ok.png")

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
	wait(2)
	type(Key.BACKSPACE)#+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE)
	click("ok.png")
	assert exists("create_database_for_this_user.png")
	if i == 1:
		type(Key.ENTER) #cancel confirmation
		assert not exists("create_database_for_this_user.png")
	if i == 2:
		click("ok.png")
		wait(5)
		click("Back_arrow.png")
		click("Next_arrow.png")
		type(Key.BACKSPACE)#+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE+Key.BACKSPACE)
		click("ok.png")
		click("ok.png")
		

wait(5)
click("Next_arrow.png")
assert exists("Please_create_one_author.png")
click("ok.png")

type(Key.TAB+Key.TAB+"First"+Key.TAB+Key.TAB+"Last"+Key.TAB+Key.ENTER)
click("ok.png")
click("Next_arrow.png")
click("Back_arrow.png")
type(Key.TAB+Key.TAB+Key.TAB+Key.ENTER)
assert exists("There_is_already_an_Author.png")
click("ok.png")
click("Next_arrow.png")

click("Create_microscope.png")
assert exists("Please_enter_name_of_microscope.png")
click("ok.png")

keyDown(Key.SHIFT)
type(Key.TAB)
keyUp(Key.SHIFT)

wait(1)
type("microscope"+Key.TAB+Key.ENTER)
assert exists("Your_microscope_created.png")
click("ok.png")
click("Create_microscope.png")
assert exists("There_is_already_microscope_name.png")
click("ok.png")
click("Finish.png")
assert exists("MySQL_user_and_DB_created.png")
click("ok.png")
assert not exists("Create_microscopes_for_gofigure_projects_page.png",0)

#click("Settings.png")
#click("Database_arrow.png")
#assert not exists("Set_up_database.png")