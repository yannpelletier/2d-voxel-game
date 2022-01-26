ceiling_light_pl = {}

function ceiling_light_pl:setProperties()
	self.showcaseName = "Ceiling light"
	self.texture = "placables/ceiling light/texture.png"
	self.icon = "placables/ceiling light/icon.png"
	self.type = "Light Source"
	self.tileSize = {1,1}
	self.placedOn = {"ceiling"}	
	self.hitpoints = 2
end

newPlacable("ceiling_light_pl")

function ceiling_light_pl:onPlaced(object)
	object:addLight()
	object:addInt("loops", 0)
	object:addInt("nextFlicker", math.random(120,900))
	local light = object:getLight()
	light:setColor(180,255,255,255)
	light:setRadius(17)
	light:setIntensity(5)
	light:setOffset(0.25,0.25)
	light:setConeAngle(45)
	light:setDirection(0,-1)
end

function ceiling_light_pl:update(object)
	local light = object:getLight()
	local intensity = light:getIntensity()
	local r,g,b,a = light:getColor()
	
	local loops = object:getValue("loops") + 1
	local nextFlicker = object:getValue("nextFlicker")
	object:setValue("loops", loops)
	
	if nextFlicker == loops then
		light:setColor(r,g,b,0)
		sound.playSFX("assets/placables/ceiling light/flicker.wav", 0)
	elseif loops > nextFlicker and loops < nextFlicker + 3 then
		light:setColor(r,g,b,255)
	elseif loops > nextFlicker + 3 and loops < nextFlicker + 6 then
		light:setColor(r,g,b,0)
	elseif loops > nextFlicker + 6 and loops < nextFlicker + 9 then
		light:setIntensity(5)
		light:setColor(r,g,b,255)
		object:setValue("loops", 0)
		object:setValue("nextFlicker", math.random(120,900))
	end
end