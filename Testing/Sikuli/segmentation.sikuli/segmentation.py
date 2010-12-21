r = Pattern("1292950998107.png").similar(0.28)
findAll(Pattern("1292950887474.png").similar(.75))
mm = getLastMatches()
while mm.hasNext():
	click(mm.next())




r =find("1292950998107.png")
print r
print r.getX()
findAll(Pattern("1292950887474.png").similar(.75))
SCREEN.setROI(r.getX(),r.getY(),r.getW(),r.getH())
mm=SCREEN.getLastMatches()

while mm.hasNext():
	click(mm.next())
