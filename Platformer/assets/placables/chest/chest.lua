wooden_chest_pl = {}

function wooden_chest_pl:setProperties()
	--Used in the game
	self.showcaseName = "Wooden Chest"
	self.texture = "placables/chest/texture.png"
	self.icon = "placables/chest/icon.png"
	self.type = "Container"
	self.tileSize = {2,2}
	self.placedOn = {"floor"}	
	self.spritesheetSize = {4,2}
	self.animations = {
		{"default",3,3,5000},
		{"opening",4,7,55},
		{"closing",0,3,55}
	}
	self.hitpoints = 2

end

newPlacable("wooden_chest_pl")

function wooden_chest_pl:onPlaced(object)
	object:addBool("wasClosed", true)
	object:initInventory("longLarge")
		
	object:addPlatform("top",0,1,2)
end

function wooden_chest_pl:update(object)
	local wasClosed = object:getValue("wasClosed")
	local inventoryClosed = object:isInventoryClosed()
	
	if inventoryClosed and wasClosed == false then
		object:setValue("wasClosed", true)
		object:setAnimation("closing" ,1)

	end
	
end

function wooden_chest_pl:use(object) --When pressing e on the item as it is placed in the world
	object:openInventory()
	object:setValue("wasClosed", false)
	object:setAnimation("opening",1)


end