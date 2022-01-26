thunderstorm_ef = {}

function thunderstorm_ef:setProperties()
	self.texture = "effects/thunderstorm/texture.png"
	self.size = {500,3000}
	self.spritesheetSize = {11,1}
	self.animations = {
		{"default",0,10,50},
	}
	
end

newEffect("thunderstorm_ef")

function thunderstorm_ef:onCreate(effect)
	effect:addTimer("lifeTimer")
end

function thunderstorm_ef:update(effect)
	local lifeTimer = effect:getValue("lifeTimer")
	local lifeTime = lifeTimer:getTime()
	
	if lifeTime >= 550 then
		effect:destroy()
	end
end

function thunderstorm_ef:onContact(effect, collidedWith, collidedType)
	if collidedType == "agent" then
		collidedWith:damage(500)
	end
end