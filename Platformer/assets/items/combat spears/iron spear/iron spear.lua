iron_spear_ui = {}

function iron_spear_ui:setProperties()
	--Used in the game
	self.showcaseName = "Iron spear"
	self.texture = "items/combat spears/iron spear/texture.png"
	self.icon = "items/combat spears/iron spear/icon.png"
	self.type = "Spear"
	self.itemSize = {5,100}
	self.drawScale = {1.75,1}
	self.offset = {0,0}
	self.angleOffset = -60
	self.spritesheetSize = {1,1}
	self.animations = {
		{"default",0,0,5000}
	}
	self.twoHanded = true
	
	self.spinDelay = 250
	self.stabDelay = 650
	self.knockBackForce = 1500
end

newUsableItem("iron_spear_ui")

--Called when the item gets equipped
function iron_spear_ui:onEquip(heldItem, mousePos)
	heldItem:addTimer("useTimer")
	heldItem:addTimer("damageTimer")
		
	heldItem:addNum("mouseX", 0)
	heldItem:addNum("mouseY", 0)
end

--Called when the item gets unequipped
function iron_spear_ui:onUnequip(heldItem, mousePos)

end

--Called while the item is equipped
function iron_spear_ui:whileEquipped(heldItem, mousePos)
	local useTimer = heldItem:getValue("useTimer")
	local useTime = useTimer:getTime()
		
	if heldItem:getArmAnimationName() == "holding" then
		if input.isKeyDown("MOUSE_LEFT") then
			if useTime >= self.stabDelay - 25 then
				heldItem:playAnimation("stab", self.stabDelay)
				useTimer:reset()
			end
		elseif input.isKeyDown("MOUSE_RIGHT") then
			if useTime >= self.spinDelay - 25 then
				heldItem:playAnimation("spin", self.spinDelay)
				heldItem:setOffset(0,0)
				useTimer:reset()
			end
		end
	end
	
	--Make an animation to hold the spear from the tip when stabbing
	if heldItem:getArmAnimationName() == "stabbing" then
		if useTime < self.stabDelay / 2 then
			heldItem:setOffset(0, math.round(math.interpolate(0,40,useTime, self.stabDelay / 2)))
			heldItem:setAngleOffset(math.round(math.interpolate(self.angleOffset,self.angleOffset + 65,useTime, self.stabDelay / 2)))
		else
			if useTime <= self.stabDelay then
				local scaledTime = ((self.stabDelay / 2) - (useTime - (self.stabDelay / 2)))
				heldItem:setOffset(0, math.round(math.interpolate(40,0,useTime - (self.stabDelay / 2), self.stabDelay / 2)))
				heldItem:setAngleOffset(math.round(math.interpolate(self.angleOffset + 65,self.angleOffset,useTime - (self.stabDelay / 2), self.stabDelay / 2)))
			end
		end
	end
		
	heldItem:setValue("mouseX", mousePos[1])
	heldItem:setValue("mouseY", mousePos[2])
end

function iron_spear_ui:onContact(heldItem, collidedWith, collidedType)
	if heldItem:getArmAnimationName() == "stabbing" then
	
		local holderPosition = heldItem:getHolder():getPosition()
		
		if heldItem:getValue("damageTimer"):getTime() >= self.stabDelay then
			if heldItem:getEventState("swinging") then
				if collidedType == "agent" then
					local collidedWithPosition = collidedWith:getPosition()
					if holderPosition[1] >= collidedWithPosition[1] then
						collidedWith:knockBack(math.toRadian(135),self.knockBackForce)
					else
						collidedWith:knockBack(math.toRadian(45),self.knockBackForce)
					end
					collidedWith:damage(35)
					heldItem:getValue("damageTimer"):reset()
				end
			end
		end
	else 
		local holderPosition = heldItem:getHolder():getPosition()
		
		if heldItem:getValue("damageTimer"):getTime() >= self.spinDelay / 2 then
			if heldItem:getEventState("swinging") then
				if collidedType == "agent" then
					local collidedWithPosition = collidedWith:getPosition()
					if holderPosition[1] >= collidedWithPosition[1] then
						collidedWith:knockBack(math.toRadian(135),500)
					else
						collidedWith:knockBack(math.toRadian(45),500)
					end
					collidedWith:damage(5)
					heldItem:getValue("damageTimer"):reset()
				end
			end
		end
	end
end
