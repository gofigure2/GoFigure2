

Toolbar = ("default","ZoomIcon.png","PanIcon.png","ActorPickingIcon.png")

for mode in Toolbar:

	 if mode != "default":
	 	click(mode)

	#button: left, right, middle, scroll
	#action: single_click, double_click, drag_up2down,drag_down2up,drag_left2right,drag_right2left
	#view: xy, xz, yz, xyz 	