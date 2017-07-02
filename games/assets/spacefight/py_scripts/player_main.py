import spacefight

player_game_object_id = 0

# This method gets called every frame.
# Here, the main logic for the player character 
# is processed.

def doFrame():
	h = spacefight.get_player_health(1)
	# if (spacefight.key_down(KEY_A) then:
	spacefight.game_object_translate(player_game_object_id, 1, 0, 0)
	return h

# This method gets called by the engine once at
# initialization time. 
# Here, we spawn our player character game object and 
# store the id we get from the engine.	
def initGameModule():
	global player_game_object_id
	# Spawn at world center and give it the name "player"
	player_game_object_id = spacefight.spawn_game_object("player", 0, 0, 0)
	return 0