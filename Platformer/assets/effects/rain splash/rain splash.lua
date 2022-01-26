rain_splash_ef = {}

function rain_splash_ef:setProperties()
	self.texture = "effects/rain splash/texture.png"
	self.size = {12,12}
	self.spritesheetSize = {5,1}
	self.animations = {
		{"default",0,4,50},
	}
	
end

newEffect("rain_splash_ef")

function rain_splash_ef:onCreate(effect)
	effect:addTimer("lifeTimer")
end

function rain_splash_ef:update(effect)
	local lifeTimer = effect:getValue("lifeTimer")
	local lifeTime = lifeTimer:getTime()
	
	if lifeTime >= 250 then
		effect:destroy()
	end
end

function rain_splash_ef:onContact(effect, collidedWith, collidedType)
	
end