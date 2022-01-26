stone_m = {}

function stone_m:setProperties()
	self.showcaseName = "Stone"
	self.texture = "materials/stone/texture.png"
	self.icon = "materials/stone/icon.png"
	self.particle = "generic"
	self.particleColor = {127,127,127,255}
	self.stepSound = nil
	self.solid = true
	self.autoTile = true
	self.ground = true
	self.spritesheetSize = {16,1}
	self.hitpoints = 6
	self.miningSounds = {"assets/materials/stone/hitsound1.wav", "assets/materials/stone/hitsound2.wav", "assets/materials/stone/hitsound3.wav"}

end

newMaterial("stone_m")

function stone_m:onHit(hitObject, collidedType)
	
end