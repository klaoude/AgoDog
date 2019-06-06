var PlayerTracker = require('../PlayerTracker');
var gameServer = require('../GameServer');
var Vector = require('vector2-node');

function BotPlayer() {
    PlayerTracker.apply(this, Array.prototype.slice.call(arguments));
    //this.color = gameServer.getRandomColor();

    this.splitCooldown = 0;
}

module.exports = BotPlayer;
BotPlayer.prototype = new PlayerTracker();

// Functions

BotPlayer.prototype.getLowestCell = function() {
    // Gets the cell with the lowest mass
    if (this.cells.length <= 0) {
        return null; // Error!
    }

    // Sort the cells by Array.sort() function to avoid errors
    var sorted = this.cells.valueOf();
    sorted.sort(function(a, b) {
        return b.mass - a.mass;
    });

    return sorted[0];
};

BotPlayer.prototype.update = function() { // Overrides the update function from player tracker
    // Remove nodes from visible nodes if possible
    for (var i = 0; i < this.nodeDestroyQueue.length; i++) {
        var index = this.visibleNodes.indexOf(this.nodeDestroyQueue[i]);
        if (index > -1) {
            this.visibleNodes.splice(index, 1);
        }
    }

    // Respawn if bot is dead
    if (this.cells.length <= 0) {
        this.gameServer.gameMode.onPlayerSpawn(this.gameServer, this);
        if (this.cells.length == 0) {
            // If the bot cannot spawn any cells, then disconnect it
            this.socket.close();
            return;
        }
    }
    
    if (this.splitCooldown > 0) this.splitCooldown--;
    
    setTimeout(function() {
        // Calculate nodes
        this.visibleNodes = this.calcViewBox();

        // Calc predators/prey
        var cell = this.getLowestCell();

        // Action
        this.decide(cell);

        // Reset queues
        this.nodeDestroyQueue = [];
        this.nodeAdditionQueue = [];
    }.bind(this), 0);
};

BotPlayer.prototype.inZone = function (x,y) {
//	console.log("border="+this.gameServer.config.borderRight+" "+this.gameServer.config.borderLeft+" "+this.gameServer.config.borderBottom+" "+this.gameServer.config.borderTop); 
	var center= new Vector (x-this.gameServer.config.borderLeft, (this.gameServer.config.borderBottom+this.gameServer.config.borderTop)/2-y); 
	return (center.length() < (this.gameServer.config.borderRight-this.gameServer.config.borderLeft)/10) ; 
	//return ( (x<(this.gameServer.config.borderRight-this.gameServer.config.borderLeft)/10) && ((y>40/100*(this.gameServer.config.borderBottom-this.gameServer.config.borderTop)) && (y<60/100*(this.gameServer.config.borderBottom-this.gameServer.config.borderTop))))
} 

// Custom
BotPlayer.prototype.decide = function(cell) {
    if (!cell) return; // Cell was eaten, check in the next tick (I'm too lazy)
    
    var cellPos = cell.position;
    var result = new Vector(0, 0);
    

    for (var i = 0; i < this.visibleNodes.length; i++) {
	var check = this.visibleNodes[i];
	var checkPos = check.position;
	var displacement = new Vector(checkPos.x - cellPos.x, checkPos.y - cellPos.y);

	// Figure out distance between cells
	var distance = displacement.length();
	if (check.cellType==0) {
		if ( distance < check.owner.playerType.range ) {
			//console.log("I ("+this.inZone(cellPos.x,cellPos.y)+") see :"+cellPos.x+" "+cellPos.y+" range="+check.owner.playerType.range+"distance="+distance); 
			// Produce force vector exerted by this entity on the cell
			if ( check.owner.playerType.special || !this.inZone(cellPos.x,cellPos.y)) {
				var force = displacement.normalize().scale(-check.owner.playerType.range/distance);
				result.add(force);
			}
		}
	}
    }	 
    // Normalize the resulting vector
    if (result.x==0 && result.y==0 && this.inZone(cellPos.x,cellPos.y)) {
    	this.mouse = {
       		x: this.gameServer.config.borderLeft,
        	y: (this.gameServer.config.borderTop+this.gameServer.config.borderBottom)/2
    	};
	
    }
    else {
        result.normalize(); 
    	this.mouse = {
       		x: cellPos.x + result.x * 800,
        	y: cellPos.y + result.y * 800
    	};
    }
};

// Subfunctions

BotPlayer.prototype.largest = function(list) {
    // Sort the cells by Array.sort() function to avoid errors
    var sorted = list.valueOf();
    sorted.sort(function(a, b) {
        return b.mass - a.mass;
    });

    return sorted[0];
};

BotPlayer.prototype.splitDistance = function(cell) {
    // Calculate split distance and check if it is larger than the raw distance
    var mass = cell.mass;
    var t = Math.PI * Math.PI;
    var modifier = 3 + Math.log(1 + mass) / 10;
    var splitSpeed = cell.owner.gameServer.config.playerSpeed * Math.min(Math.pow(mass, -Math.PI / t / 10) * modifier, 150);
    var endDist = Math.max(splitSpeed * 12.8, cell.getSize() * 2); // Checked via C#, final distance is near 6.512x splitSpeed
    
    return endDist;
};
