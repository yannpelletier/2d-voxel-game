grass_mm = {}

function grass_mm:setProperties()
	self.texture = "material mods/grass/texture.png"
	self.stepSound = nil
	self.autoTile = true
	self.spritesheetSize = {16,1}
	self.animations = {
		{"default",0,0,5000}
	}
	self.hitpoints = 2
	
	self.lowestGrowTime = 60000
	self.highestGrowTime = 180000
	
end

newMaterialMod("grass_mm")

function grass_mm:onPlaced(materialMod, tile)
	materialMod:addTimer("grow")
	materialMod:addInt("nextGrow", math.random(self.lowestGrowTime, self.highestGrowTime))
end

function grass_mm:update(materialMod, tile)
	local timer = materialMod:getValue("grow")
	local pos = tile:getPosition()
	
	if timer:getTime() >= materialMod:getValue("nextGrow") then		
		local leftTile = world.getTile(pos[1] - 1, pos[2])
		if self:isTileGrassable(leftTile, false) then
			leftTile:setFrontMaterialMod("grass_mm")
		else 
			leftTile = world.getTile(pos[1] - 1, pos[2] + 1)
		
			if self:isTileGrassable(leftTile, false) then
				leftTile:setFrontMaterialMod("grass_mm")
				
			else 
				leftTile = world.getTile(pos[1] - 1, pos[2] - 1)
				
				if self:isTileGrassable(leftTile, false) then
					leftTile:setFrontMaterialMod("grass_mm")
				end
			end
		end
		
		local rightTile = world.getTile(pos[1] + 1, pos[2])
		if self:isTileGrassable(rightTile, true) then
			rightTile:setFrontMaterialMod("grass_mm")
		else 
			rightTile = world.getTile(pos[1] + 1, pos[2] + 1)
		
			if self:isTileGrassable(rightTile, true) then
				rightTile:setFrontMaterialMod("grass_mm")
				
			else 
				rightTile = world.getTile(pos[1] + 1, pos[2] - 1)
				
				if self:isTileGrassable(rightTile, true) then
					rightTile:setFrontMaterialMod("grass_mm")
				end
			end
		end
		
		timer:reset()
		materialMod:setValue("nextGrow", math.random(self.lowestGrowTime, self.highestGrowTime))
	end
end

function grass_mm:onDestroy(materialMod, tile)
	
end

function grass_mm:isTileGrassable(tile, right)
	if right then
		if tile == nil then
			return false
		else
			local pos1 = tile:getPosition()
			local t = _G[tile:getFrontMaterial()]
			
			if(t ~= nil) then
				if tile:frontIsModdable() and t.ground then
					if world.getTile(pos1[1], pos1[2] + 1) == nil then
						return true
					else
						local hasFront = world.getTile(pos1[1], pos1[2] + 1):frontIsModdable()
						
						if hasFront == true then 
							return false
						else
							return true
						end
					end
				else
					return false
				end
			end
		end
	else
		if tile == nil then
			return false
		else
			local pos1 = tile:getPosition()
			local s = tile:getFrontMaterialMod()
			local t = _G[tile:getFrontMaterial()]
			
			if(t ~= nil) then
				if tile:frontIsModdable() and t.ground then
					if world.getTile(pos1[1], pos1[2] + 1) == nil then
						if world.getTile(pos1[1] - 1, pos1[2] + 1):getFrontMaterialMod() ~= "grass_mm" and
						world.getTile(pos1[1] - 1, pos1[2]):getFrontMaterialMod() ~= "grass_mm" and
						world.getTile(pos1[1] - 1, pos1[2] - 1):getFrontMaterialMod() ~= "grass_mm" then
							return true
						else 
							return false
						end
					else
						local hasFront = world.getTile(pos1[1], pos1[2] + 1):frontIsModdable()
						
						if hasFront == true then 
							return false
						else
							if world.getTile(pos1[1] - 1, pos1[2] + 1):getFrontMaterialMod() ~= "grass_mm" and
							world.getTile(pos1[1] - 1, pos1[2]):getFrontMaterialMod() ~= "grass_mm" and
							world.getTile(pos1[1] - 1, pos1[2] - 1):getFrontMaterialMod() ~= "grass_mm" then
								return true
							else 
								return false
							end
						end
					end
				else
					return false
				end
			end
		end
	end
end