# Puzzlescript Wrapper

This project provides a way to distribute a [Puzzlescript](https://www.puzzlescript.net/) game as a standalone application.

You can download a binary version of the project on [lafaceobscuredupixel.fr](http://lafaceobscuredupixel.fr/#/labo/puzzlescriptWrapper)

You can find the [official Puzzlescript repository here](https://github.com/increpare/PuzzleScript).

PuzzlescriptWrapper is using the c++/SDL2/Opengl combo.

I didn't rewrite puzzlescript engine in C++. The javascript engine is embeded so it can be replaced or updated easily (see ./data/js/puzzlescript/ sub-directory)

PuzzlescriptWrapper is published under the terms of the LGPL v3 license (See both [GPL](https://www.gnu.org/licenses/gpl.txt) and [LGPL](https://www.gnu.org/licenses/lgpl.txt) Licenses for more details)

## v2.0.3

- embeded puzzlescript engine updated to v1.7

## v2.0.2

- Adding music support: if you used the youtube meta in your puzzlescript game, add an audio file in ./data/music/ with the youtube id as the filename.
- sound from file: if you want to replace the puzzlescript generated sounds by some custom sounds, add the audio files in ./data/sfx/ with the puzzlescript sound code as  the filename.
- HUD widgets :
	- toggle fullscreen
	- close application
	- set music and sound level
	- open/close game list

## v2.0.1

- Toggle fullscreen with Alt+Enter
- Change window title on game selection
- embeded puzzlescript engine updated to v1.6

## v2.0.0

This is a rewrite of PuzzlescriptWrapper using visual studio and v8 javascript engine.

- Performances allow playing real time puzzlescript games without any framerate issue
- You can choose the game you want to play in a list (hitting TAB key to show/hide the list)
- If there is only one game in the ./data/games/ directory, the game list feature is disabled. So you can distribute a standalone version of your game without any compilation or modification in the puzzlescriptWrapper project.
- Basic game controller support (tested with xbox 360 controller but should work with most pc compatible controllers)
- show/hide framerate by hitting 'i' key
- fullscreen mode and resolution can be setup from the config.json file
- When you resize the window, the game is now resized accordingly

## v0.1.1

For this version, I tried a lot of games and fixed some bugs that prevent some of them to run correctly.
	
### Content

- Sfx emulation almost work (the sound is playing to fast but is mostly the same
- short web color format like "#af0" is now working
- fix some sprite display
	
## v0.1.0

### Content

- Most of puzzlescript games should be playable.
- The game progression is saved locally
- Sfx emulation is not finished yet
- realtime games can be slow

### Detail

[x]	Loading puzzlescript game from ./data/games/game
[x]	Emulate the HTML canvas to allow puzzlescript to draw sprites in SDL2/Opengl context
[x]	Display currentLevel in SDL2/Opengl context
[x]	Send keyboard inputs to puzzlescript engine
[x]	Calling puzzlescript update() function according to setInterval
[x]	puzzlescript prelude parameters: realtime_interval zoomscreen flickscreen background_color text_color
[x]	saving/loading progression in ./data/localStorage
[x]	it's now possible to pass a game in command line:

- if you specify a game file name it will search it in ./data/games/
- by default, PuzzlescriptWrapper launch ./data/games/game
		
[x]	Show/Hide Framerate by hitting TAB key

[]	Sfx generation:

- The sound generator does not simulate correctly puzzlescript's one yet
- If you want to give it a try anyway (or if you want to fix it), you can set sfx to 'ON' in config.xml

### Performance Note

I'm using [Duktape](http://duktape.org/) as the javascript engine to keep it simple and compact. Since Duktape is a non-JIT javascript engine, it is slower than Google v8 or Mozilla SpiderMonkey. So, if you are running a realtime Puzzlescript game with many rules, you will probably encounter some framerate issues.

#### Possible solutions:

- Rewrite part of puzzlescript engine in C++. It's probably a bad idea because processInput() function in puzzlescript/engine.js seems to be basically the whole engine.
- Switching to spidermonkey or v8
- Waiting for Duktape to implement the JIT feature
