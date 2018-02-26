//
// Created by eric on 2/25/18.
//

#pragma once

//assume AI has goal to destroy main control building of player
/*we do the tree thing like alla suggested
with look ahead to a depth level of 8 for example, 2^8 = 256
there we look for possible group actions
scan through the list of known player buildings with a attack action
same thing but deny resources
patrol/scout what player is doing

*/

//level 1: macro ai
//ai manages its resources -- attack groups, denying area, fortifying areas (eg resources), destroying player buildings, expanding vision
//decides on area owned (by ai), sum of the value of its units, sum of the value of the players units,
//number of resource extractors owned by player (can be included in previous sum),


//level 2: micro ai
//ai manages an individual attack group -- which one to fall back when hurt, suicide bomb, which tower/building/unit to attack
//decides based on unit health, energy, and attackpower, attackrange. enemy defence towers and resource towers have highest value
//1 squad leader per group, but 1 general/commander for the big view
// right, in which case we do path planning once and the group will follow
// or the group has a target (building, unit, etc), and they path to that as a whole?

// should we think about something like Line of sight (LOS), this is going to be local strategy I think (state machine based)
//also the commander ai will still be line of sight based as well i think, because that way it wont attack you the moment you place
//down a new building

class Ai {






};


