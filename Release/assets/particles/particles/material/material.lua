material_p = {}

function material_p:setProperties()
	self.texture = "particles/particles/material/texture.png"
	self.maxParticles = 500
	self.decayRate = 0.7
	
	self.speed = 0.288
end

newParticle("material_p")

function material_p:update(particle)
	local lifeTime = particle:getLifeTime()
	local xv, yv = particle:getVelocity()
	local vec = {xv, yv}
	local dir = vec2.norm(vec)
	local r,g,b,a = particle:getColor()
	
	
	particle:setVelocity(xv, yv - self.speed)
	particle:setColor(r,g,b,255 * lifeTime)
end