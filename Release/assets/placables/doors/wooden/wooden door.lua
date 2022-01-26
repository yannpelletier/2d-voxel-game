wooden_door_pl = {}

function wooden_door_pl:setProperties()
	--Used in the game
	self.showcaseName = "Wooden door"
	self.texture = "placables/doors/wooden/texture.png"
	self.icon = "placables/doors/wooden/icon.png"
	self.type = "Door"
	self.tileSize = {3,4}
	self.placedOn = {"floor"}	
	self.spritesheetSize = {3,1}
	self.animations = {
		{"default",0,0,10000},
		{"openedLeft",1,1,10000},
		{"openedRight",2,2,10000}
	}
	self.hitpoints = 5

end

newPlacable("wooden_door_pl")

function wooden_door_pl:onPlaced(object)
	object:addBody("doorBody", 1,0,1,4)
	object:addBool("open", false)
end

function wooden_door_pl:update(object)
	
end

function wooden_door_pl:use(object, user) --When pressing e on the item as it is placed in the world
	if object:getValue("open") == false then
		local position = {}
		position = object:getCenterPosition()
		local userPosition = {}
		userPosition = user:getPosition()
	
		object:setValue("open", true)
		object:getValue("doorBody"):setSolid(false)
		sound.playSFX("assets/placables/doors/wooden/open.wav", 0)
		
		if userPosition[1] >= position[1] then
			object:setAnimation("openedLeft")
		else
			object:setAnimation("openedRight")
		end
	else
		object:setValue("open", false)
		object:getValue("doorBody"):setSolid(true)
		sound.playSFX("assets/placables/doors/wooden/close.wav", 0)
		object:setAnimation("default")
	end
end