William Elmes
welmes@csu.fullerton.edu
12/6/2010

Tower Defense

A basic tower defense game in which enemies start on one side of a two-dimensional grid and move across it along a simple closed path to a destination space on the opposite side of the grid. The player builds towers along the enemy's path. Towers are immobile automated structures that attack enemies within their attack range. Each enemy that is killed grants the player a certain amount of money, which can be used to build more or upgrade already built towers. Every time an enemy reaches their end destination before the player's towers can kill it, the player loses a life and the enemy is teleported back tot he start location to traverse the path again. The goal of the game is to build your towers strategically in order to survive through all the waves of enemies of each level without losing all of your lives. 

I've left on a few extra cheats for testing purposes in addition to the normal controls:
Up/down arrows increase/decrease your money respectively.
Left arrow loads level 1, and right arrow loads level 2.

In order to compile link against the following libraries:
SDL.lib
SDLmain.lib
SDL_image.lib
SDL_ttf.lib
located in the  Tower Defense\Tower Defense\lib  folder
and include the  Tower Defense\Tower Defense\SDL  folder.
DLL files needed to run are located in the  Tower Defense\Tower Defense  folder.

An executable file named "Tower Defense.exe" can be run straight out of the  Tower Defense\Tower Defense  folder