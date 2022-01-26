ground_m = {}

function ground_m:setProperties()
	self.showcaseName = "Ground"
	self.texture = "materials/ground/texture.png"
	self.icon = "materials/ground/icon.png"
	self.particle = "generic"
	self.particleColor = {150,108,73,255}
	self.stepSound = nil
	self.solid = true
	self.autoTile = true
	self.ground = true
	self.spritesheetSize = {16,1}
	self.hitpoints = 3
	self.miningSounds = {"assets/materials/ground/hitsound1.wav","assets/materials/ground/hitsound2.wav","assets/materials/ground/hitsound3.wav"}

end

newMaterial("ground_m")

function ground_m:onHit(hitObject, collidedType)
	
end