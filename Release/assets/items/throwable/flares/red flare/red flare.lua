red_flare_ui = {}

function red_flare_ui:setProperties()
	--Used in the game
	self.showcaseName = "Red flare"
	self.texture = "items/throwable/flares/red flare/texture.png"
	self.icon = "items/throwable/flares/red flare/icon.png"
	self.type = "Utility"
	self.itemSize = {20,5}
	self.offset = {0,0}
	self.angleOffset = 0
	self.spritesheetSize = {1,1}
	self.animations = {
		{"default",0,0,5000}
	}
	self.twoHanded = false
end

newUsableItem("red_flare_ui")

--Called when the item gets equipped
function red_flare_ui:onEquip(heldItem, mousePos)
	
end

--Called while the item is equipped
function red_flare_ui:whileEquipped(heldItem, mousePos)
	local positionVec = heldItem:getPosition()
	local directionVec = {}
	directionVec[1] = mousePos[1] - positionVec[1]
	directionVec[2] = mousePos[2] - positionVec[2]
	directionVec = vec2.norm(directionVec)
	
	local heldItemAngle = vec2.angleDeg(heldItem:getDirection())
	local calculatedAngle = vec2.angleDeg(directionVec)
	
	--Otherwise, if the cursor is behind the bullet goes backwards
	if calculatedAngle <= heldItemAngle + 20 and calculatedAngle >= heldItemAngle - 20 then
	else
		directionVec[1] = math.cos(heldItemAngle * (2 * math.pi) / 360)
		directionVec[2] = math.sin(heldItemAngle * (2 * math.pi) / 360)
	end
	
	if heldItem:getArmAnimationName() == "holding" then
		if input.keyPressed(heldItem:getUseButton()) then
			heldItem:createProjectile("red_flare_pr",0,0,directionVec[1] * 500, directionVec[2] * 500)
			heldItem:consume()
		end
	end
	
end
