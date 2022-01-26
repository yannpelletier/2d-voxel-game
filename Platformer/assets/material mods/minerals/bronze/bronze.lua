bronze_mm = {}

function bronze_mm:setProperties()
	self.texture = "material mods/minerals/bronze/texture.png"
	self.stepSound = nil
	self.autoTile = false
	self.spritesheetSize = {4,1}
	self.animations = {
		{"variation1",0,0,5000},
		{"variation2",1,1,5000},
		{"variation3",2,2,5000},
		{"variation4",3,3,5000}
	}
	self.hitpoints = 6
	self.miningSounds = {"assets/material mods/minerals/hitsound1.wav", "assets/material mods/minerals/hitsound2.wav", "assets/material mods/minerals/hitsound3.wav", "assets/material mods/minerals/hitsound4.wav"}
	
end

newMaterialMod("bronze_mm")

function bronze_mm:onPlaced(materialMod, tile)
	materialMod:setAnimation("variation" .. math.random(1,4))
end

function bronze_mm:update(materialMod, tile)

end

function bronze_mm:onDestroy(materialMod, tile)
	tile:drop("bronze_ui")
end
