import spacefight
import myfuncs

gv = 3

# This gets called from the engine 
# and it is expected, that all additional 
# modules are registered here.
# A module must be in the same folder as 
# this script and its name must correspond to its file name.
# A game module script must have two methods, which will be called by the engine:
# - initGameModule
# - doFrame

def initModules():
	spacefight.register_game_module("player_main")
	#spacefight.register_game_module("main_boss_ai")
	#spacefight.register_game_module("basic_enemy_ai")
	#spacefight.register_game_module("enemy_spawner")
	
	
	
	


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
	spacefight.spawn_game_object("mygo_hero2")
		
	return a + 99 + h + (c.f()) + s.go() + gv + myfuncs.hello()
	

def onFirePressed():
	return 99
