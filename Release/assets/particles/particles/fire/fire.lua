fire_p = {}

function fire_p:setProperties()
	self.texture = "particles/particles/fire/texture.png"
	self.maxParticles = 500
	self.decayRate = 0.7
	
	self.speed = 0.05
end

newParticle("fire_p")

function fire_p:update(particle)
	local lifeTime = particle:getLifeTime()
	local xv, yv = particle:getVelocity()
	local vec = {xv, yv}
	local dir = vec2.norm(vec)
	local r,g,b,a = particle:getColor()
	
	
	particle:setVelocity(dir[1] * self.speed * lifeTime, yv - (0.001))
	particle:setColor(r,g,b,255 * lifeTime)
end