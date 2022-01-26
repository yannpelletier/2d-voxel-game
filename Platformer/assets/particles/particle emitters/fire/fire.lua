fire_pe = {}

function fire_pe:setProperties()
	self.frequence = {1000,8000}
	self.speed = {1.28, 1.6}
	self.width = {2.24, 3}
	self.angle = {60,120}
end

newParticleEmitter("fire_pe")  