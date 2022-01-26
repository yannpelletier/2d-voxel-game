red_flag_pl = {}

function red_flag_pl:setProperties()
	--Used in the game
	self.showcaseName = "Red flag"
	self.texture = "placables/decoratives/flags/red/texture.png"
	self.icon = "placables/decoratives/flags/red/icon.png"
	self.type = "Decorative"
	self.tileSize = {2,3}
	self.placedOn = {"floor"}	
	self.spritesheetSize = {5,1}
	self.animations = {
		{"default",0,4,150},
	}
	self.hitpoints = 2

end

newPlacable("red_flag_pl")

function red_flag_pl:onPlaced(object)
	
end

function red_flag_pl:update(object)

	
end
