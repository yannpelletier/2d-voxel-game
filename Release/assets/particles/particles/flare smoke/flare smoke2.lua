flare_smoke_2 = {}

function flare_smoke_2:setProperties()
	self.texture = "particles/particles/flare smoke/texture2.png"
	self.maxParticles = 40
	self.decayRate = 0.7
	
	self.speed = 0.05
end

newParticle("flare_smoke_2")

function flare_smoke_2:update(particle)
	local lifeTime = particle:getLifeTime()
	local xv, yv = particle:getVelocity()
	local vec = {xv, yv}
	local dir = vec2.norm(vec)
	local r,g,b,a = particle:getColor()
	
	local scaledLifeTime = 1 - lifeTime
	
	particle:setVelocity(0, 1)
	particle:setColor(r,g,b,math.seeSaw(0,255,scaledLifeTime,0.5,1))
end