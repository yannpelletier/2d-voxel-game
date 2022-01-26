throwable_spear_ui = {}

function throwable_spear_ui:setProperties()
	--Used in the game
	self.showcaseName = "Throwable Spear"
	self.texture = "items/throwable/spear/texture.png"
	self.icon = "items/throwable/spear/icon.png"
	self.type = "Throwable"
	self.itemSize = {50,10}
	self.offset = {0,0}
	self.angleOffset = 0
	self.spritesheetSize = {1,1}
	self.animations = {
		{"default",0,0,5000}
	}
	self.twoHanded = false
	self.throwForce = 1000
end

newUsableItem("throwable_spear_ui")

--Called when the item gets equipped
function throwable_spear_ui:onEquip(heldItem, mousePos)
	
end

--Called while the item is equipped
function throwable_spear_ui:whileEquipped(heldItem, mousePos)
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
			heldItem:createProjectile("spear_pr",0,0,directionVec[1] * self.throwForce, directionVec[2] * self.throwForce)
			heldItem:consume()
		end
	end
	
end
