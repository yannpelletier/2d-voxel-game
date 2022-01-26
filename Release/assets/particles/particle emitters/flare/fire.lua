flare_smoke_pe = {}

function flare_smoke_pe:setProperties()
	self.frequence = {100,200}
	self.speed = {1.28, 1.6}
	self.width = {25, 30}
	self.angle = {90,90}
end

newParticleEmitter("flare_smoke_pe")  