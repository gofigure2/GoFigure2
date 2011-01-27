keyDown(Key.ALT+Key.SHIFT)
type("t")
keyUp(Key.ALT+Key.SHIFT)
wait(4)

type("mysql -u gofigure -p"+Key.ENTER)
wait(2)
type("gofigure"+Key.ENTER)
wait(3)

type("drop database gofiguredatabase;"+Key.ENTER)
wait(2)

#log out
keyDown(Key.CTRL)
type("d")
wait(1)
type("d")
keyUp(Key.CTRL)

