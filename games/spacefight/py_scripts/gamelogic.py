import spacefight

gv = 3

class MyClass:
	i = 194
	
	def f(self):
		return self.i
	def b(self):
		return 88
		
class SecondClass:
	i = 99
	
	def go(self):
		return self.i

def doFrame(a):
	global gv
	print ('in doFrame')
	c = MyClass()
	c.i = c.i + 5
	
	gv = gv + 4
	
	s = SecondClass()
	# This callback is very expensive!
	# Seems to be expensive by itself, 
	# because the method on the c side does not do anything
	# special as of now!
	# Update: was only expensive because there I did a OutputDebugString!
	h = spacefight.get_player_health(1)
	#h = 100
	
	spacefight.draw_model()
	spacefight.spawn_game_object("mygo_hero")
		
	return a + 99 + h + (c.f()) + s.go() + gv
	

def onFirePressed():
	return 99
