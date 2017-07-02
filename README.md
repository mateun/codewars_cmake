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
```

This snippet registers 4 modules to be used by the engine. 
The engine will scan the current directory (the same where the "initModules" script is located)
for the following file names:
* player_main.py
* main_boss_ai.py
* basic_enemy_ai.py
* enemy_spawner.py

Each game module script must implement two methods:

```python
def doFrame():

def initGameModule():
```

`doFrame` is called, as the name suggests, every frame by the engine. Within this method, you should 
update your game logic. 

`initGameModule` is only called once, when the module is first loaded. The idea is that 
you do one-time initialization things here, e.g. loading 3D models for later use in game objects, 
giving useful starting values to variables etc.
