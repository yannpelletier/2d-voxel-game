torch_pl = {}

function torch_pl:setProperties()
	--Used in the game
	self.showcaseName = "Torch"
	self.texture = "placables/torch/texture.png"
	self.icon = "placables/torch/icon.png"
	self.type = "Light Source"
	self.tileSize = {1,2}
	self.placedOn = {"floor", "background", "wall"}	
	self.spritesheetSize = {3,2}
	self.animations = {
		{"default",3,5,125},
		{"off",0,2,125}
	}
	self.hitpoints = 5
	
	--Added
	self.flickerSpeed = 0.020
	self.beginRadius = 8
	self.beginIntensity = 4.5
	self.minIntensity = 4.4
	self.maxIntensity = 4.6
end

newPlacable("torch_pl")

function torch_pl:onPlaced(object)
	object:addLight()
	local light = object:getLight()
	light:setColor(255,255,255,255)
	light:setIntensity(self.beginIntensity)
	light:setRadius(self.beginRadius)
	light:setOffset(0.25,0.5)
	
	object:addBool("active", true)
	object:addBool("flickerUp", true)
	
	object:setEmitter("fire_pe")
	local emitter = object:getEmitter()
	emitter:addEmittedParticle("fire_p", {255,128,0,255})
	emitter:addEmittedParticle("fire_p", {255,108,32,255})
end

function torch_pl:update(object)
	local light = object:getLight()
	local intensity = light:getIntensity()
	local active = object:getValue("active")
	local flickerUp = object:getValue("flickerUp")
	
	if active then
		if flickerUp == true then --Flicker up
			intensity = intensity + self.flickerSpeed
			light:setIntensity(intensity)
		else --Flicker down
			intensity = intensity - self.flickerSpeed
			light:setIntensity(intensity)
		end

		if intensity > self.maxIntensity then
			object:setValue("flickerUp", false)
		elseif intensity < self.minIntensity then
			object:setValue("flickerUp", true)
		end
	end

end

function torch_pl:use(object, user) --When pressing e on the item as it is placed in the world
	local active = object:getValue("active")
	local light = object:getLight()
	local emitter = object:getEmitter()
	
	if active then
		object:setValue("active", false)
		object:setAnimation("off")
		light:setRadius(0.5)
		emitter:disable()
	else
		object:setValue("active", true)
		object:setAnimation("default")
		light:setRadius(self.beginRadius)
		emitter:enable()
	end
end

function torch_pl:whileEquipped(heldItem, mousePos)
	local light = heldItem:getLight()
	local intensity = light:getIntensity()
	local flickerUp = heldItem:getValue("flickerUp")

	if flickerUp == true then --Flicker up
		intensity = intensity + self.flickerSpeed
		light:setIntensity(intensity)
	else --Flicker down
		intensity = intensity - self.flickerSpeed
		light:setIntensity(intensity)
	end

	if intensity > self.maxIntensity then
		heldItem:setValue("flickerUp", false)
	elseif intensity < self.minIntensity then
		heldItem:setValue("flickerUp", true)
	end
end

function torch_pl:onEquip(heldItem, mousePos)
	heldItem:setAnimation("off")

	heldItem:addLight()
	local light = heldItem:getLight()
	light:setColor(255,255,255,255)
	light:setIntensity(5)
	light:setRadius(5)
	
	heldItem:addBool("flickerUp", true)
end

function torch_pl:onUnequip(heldItem, mousePos)
	
end