pickaxe_ui = {}

function pickaxe_ui:setProperties()
	--Used in the game
	self.showcaseName = "Pickaxe"
	self.texture = "items/pickaxe/texture.png"
	self.icon = "items/pickaxe/icon.png"
	self.type = "Digger"
	self.itemSize = {20,30}
	self.offset = {0,10}
	self.angleOffset = 0
	self.spritesheetSize = {1,1}
	self.animations = {
		{"default",0,0,5000}
	}
	self.twoHanded = true
	
	self.digWidth = 3;
	self.useDelay = 400
	self.digRange = 150

end

newUsableItem("pickaxe_ui")

--Called when the item gets equipped
function pickaxe_ui:onEquip(heldItem, mousePos)
	heldItem:addTimer("useTimer")
	heldItem:addBool("queuedFront", true)
	heldItem:addBool("queuedSingle", false)
end

--Called when the item gets unequipped
function pickaxe_ui:onUnequip(heldItem, mousePos)

end

function pickaxe_ui:playMiningSound(tile, front)
	if tile ~= nil then
		if front then
			local frontMaterial = tile:getFrontMaterial()
			local frontMaterialMod = tile:getFrontMaterialMod()
			local nameIDSound = nil
			local channelToPlayIn = 0
			
			if frontMaterialMod == "" then
				if frontMaterial ~= "" then
					nameIDSound = _G[frontMaterial]
					channelToPlayIn = 15
				end
			else
				nameIDSound = _G[frontMaterialMod]
				channelToPlayIn = 16
			end
			
			if nameIDSound ~= nil then
				if nameIDSound.miningSounds ~= nil then
					local soundNum = #nameIDSound.miningSounds
					sound.playSFX(nameIDSound.miningSounds[math.random(1,soundNum)], 0, channelToPlayIn)
				end
			end
		else
			local backMaterial = tile:getBackMaterial()
			local backMaterialMod = tile:getBackMaterialMod()
			local nameIDSound = nil
			local channelToPlayIn = 0
			
			if backMaterialMod == "" then
				if backMaterial ~= "" then
					nameIDSound = _G[backMaterial]
					channelToPlayIn = 15
				end
			else
				nameIDSound = _G[backMaterialMod]
				channelToPlayIn = 16
			end
			
			if nameIDSound ~= nil then
				if nameIDSound.miningSounds ~= nil then
					local soundNum = #nameIDSound.miningSounds
					sound.playSFX(nameIDSound.miningSounds[math.random(1,soundNum)], 0, channelToPlayIn)
				end
			end
		end
	end
end

--Called while the item is equipped
function pickaxe_ui:whileEquipped(heldItem, mousePos)
	local useTimer = heldItem:getValue("useTimer")
	local useTime = useTimer:getTime()
	local itemPos = heldItem:getPosition()
	local miningTargetRange = {}
	miningTargetRange[1] = mousePos[1] - itemPos[1]
	miningTargetRange[2] = mousePos[2] - itemPos[2]

	if input.isKeyDown("MOUSE_LEFT") then
		if input.isKeyDown("LSHIFT") then
			if useTime >= self.useDelay then
				heldItem:playAnimation("mine", self.useDelay)
			
				heldItem:setValue("queuedFront", true)
				heldItem:setValue("queuedSingle", true)
				
				useTimer:reset()
			end
		else
			if useTime >= self.useDelay then
				heldItem:playAnimation("mine", self.useDelay)

				heldItem:setValue("queuedFront", true)
				heldItem:setValue("queuedSingle", false)
								
				useTimer:reset()
			end
		end
	end
	
	if input.isKeyDown("MOUSE_RIGHT") then
		if input.isKeyDown("LSHIFT") then
			if useTime >= self.useDelay then
				heldItem:playAnimation("mine", self.useDelay)
			
				heldItem:setValue("queuedFront", false)
				heldItem:setValue("queuedSingle", true)		
				
				useTimer:reset()
			end
		else
			if useTime >= self.useDelay then
				heldItem:playAnimation("mine", self.useDelay)
		
				heldItem:setValue("queuedFront", false)
				heldItem:setValue("queuedSingle", false)

				useTimer:reset()
			end
		end
	end
	
	if vec2.mag(miningTargetRange) <= self.digRange then
		if heldItem:getEventState("swing end") then
			local queuedFront = heldItem:getValue("queuedFront")
			local queuedSingle = heldItem:getValue("queuedSingle")
			
			if queuedFront then
				if queuedSingle then
					local tile = world.getTile(math.floor(mousePos[1] / world.tileSize), math.floor(mousePos[2] / world.tileSize))
					self:playMiningSound(tile, true)
					world.damageFrontTile(math.floor(mousePos[1] / world.tileSize), math.floor(mousePos[2] / world.tileSize), 1)
				else
					local center = {}
					local gridBottomLeft = {}
					
					if self.digWidth % 2 == 0 then
						center[1] = math.ceil(mousePos[1] / world.tileSize)
						center[2] = math.ceil(mousePos[2] / world.tileSize)
					else
						center[1] = math.floor(mousePos[1] / world.tileSize)
						center[2] = math.floor(mousePos[2] / world.tileSize)
					end
					
					gridBottomLeft[1] = math.floor(center[1] - math.floor(self.digWidth / 2))
					gridBottomLeft[2] = math.floor(center[2] - math.floor(self.digWidth / 2))

					for x = 0, self.digWidth - 1 do
						for y = 0, self.digWidth - 1 do
							local tile = world.getTile(gridBottomLeft[1] + x, gridBottomLeft[2] + y)
							self:playMiningSound(tile, true)
							world.damageFrontTile(gridBottomLeft[1] + x, gridBottomLeft[2] + y, 1)
						end
					end
				end
			else
				if queuedSingle then
					local tile = world.getTile(math.floor(mousePos[1] / world.tileSize), math.floor(mousePos[2] / world.tileSize))
					self:playMiningSound(tile, false)
					world.damageBackTile(math.floor(mousePos[1] / world.tileSize), math.floor(mousePos[2] / world.tileSize), 1)
				else
					local center = {}
					local gridBottomLeft = {}
					
					if self.digWidth % 2 == 0 then
						center[1] = math.ceil(mousePos[1] / world.tileSize)
						center[2] = math.ceil(mousePos[2] / world.tileSize)
					else
						center[1] = math.floor(mousePos[1] / world.tileSize)
						center[2] = math.floor(mousePos[2] / world.tileSize)
					end
					
					gridBottomLeft[1] = math.floor(center[1] - math.floor(self.digWidth / 2))
					gridBottomLeft[2] = math.floor(center[2] - math.floor(self.digWidth / 2))

					for x = 0, self.digWidth - 1 do
						for y = 0, self.digWidth - 1 do
							local tile = world.getTile(gridBottomLeft[1] + x, gridBottomLeft[2] + y)
							self:playMiningSound(tile, false)
							world.damageBackTile(gridBottomLeft[1] + x, gridBottomLeft[2] + y, 1)
						end
					end
				end
			end
		end
	end
end

function pickaxe_ui:onContact(heldItem, collidedWith, collidedType)
	
end
