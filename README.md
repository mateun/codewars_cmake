# codewars

Simple D3D11 based game engine including some (now just one...) sample games 
which use the engine. 

## Main engine features
* 3D rendering
* Play sounds and music
* Input handling
* Python scripting for game logic

### 3D rendering

### Sounds and music

### Input handling

### Python scripting
The engine expects the game to follow certain conventions 
for the Python scripts to work correctly:

* Place a script called "gamelogic.py" in your games/assets/your_game/py_scripts folder. 
* Within this script, implement a method called "initModules"

Find an example below:

```python
def initModules():
	engine.register_game_module("player_main")
	engine.register_game_module("main_boss_ai")
	engine.register_game_module("basic_enemy_ai")
	engine.register_game_module("enemy_spawner")
