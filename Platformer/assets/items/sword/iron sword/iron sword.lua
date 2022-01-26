iron_sword_ui = {}

function iron_sword_ui:setProperties()
	--Used in the game
	self.showcaseName = "Iron sword"
	self.texture = "items/sword/iron sword/texture.png"
	self.icon = "items/sword/iron sword/icon.png"
	self.type = "Sword"
	self.itemSize = {10,50}
	self.drawScale = {1.75,1}
	self.offset = {0,25}
	self.angleOffset = 0
	self.spritesheetSize = {1,1}
	self.animations = {
		{"default",0,0,5000}
	}
	self.twoHanded = false
	
	self.digWidth = 2
	self.useDelays = {500,500,500,500,350,350}
	self.resetTime = 700
	self.knockBackForce = 800

end

newUsableItem("iron_sword_ui")

--Called when the item gets equipped
function iron_sword_ui:onEquip(heldItem, mousePos)
	heldItem:addTimer("useTimer")
	heldItem:addTimer("damageTimer")
	
	heldItem:addInt("comboProgression",0)
	
	heldItem:addNum("mouseX", 0)
	heldItem:addNum("mouseY", 0)
end

--Called when the item gets unequipped
function iron_sword_ui:onUnequip(heldItem, mousePos)

end

--Called while the item is equipped
function iron_sword_ui:whileEquipped(heldItem, mousePos)
	local useTimer = heldItem:getValue("useTimer")
	local useTime = useTimer:getTime()
	
	if useTime >= self.resetTime then
		heldItem:setValue("comboProgression", 0)
	end
	
	local comboProgression = heldItem:getValue("comboProgression")
	
	if input.isKeyDown(heldItem:getUseButton()) then
		if useTime >= self.useDelays[comboProgression + 1] - 25 then
			if comboProgression == 0 then
				heldItem:playAnimation("swing", self.useDelays[comboProgression + 1])
				heldItem:setValue("comboProgression", 1)
				
			elseif comboProgression == 1 then
				heldItem:playAnimation("stab", self.useDelays[comboProgression + 1])
				heldItem:setValue("comboProgression", 2)
				
			elseif comboProgression == 2 then
				heldItem:playAnimation("slash up", self.useDelays[comboProgression + 1])
				heldItem:setValue("comboProgression", 3)
				
			elseif comboProgression == 3 then
				heldItem:playAnimation("stab", self.useDelays[comboProgression + 1])
				heldItem:setValue("comboProgression", 0)
				
			end
			useTimer:reset()
		end
	end
	
	heldItem:setValue("mouseX", mousePos[1])
	heldItem:setValue("mouseY", mousePos[2])
end

function iron_sword_ui:onContact(heldItem, collidedWith, collidedType)
	if heldItem:getValue("damageTimer"):getTime() >= 500 then
		if heldItem:getEventState("swinging") then
			if collidedType == "agent" then
				collidedWith:damage(35)
				heldItem:getValue("damageTimer"):reset()
				
				local agentPosition = collidedWith:getPosition()
				local holderPosition = heldItem:getHolder():getPosition()
				local distanceVec = {}
				distanceVec[1] = agentPosition[1] - holderPosition[1]
				distanceVec[2] = agentPosition[2] - holderPosition[2]
				
				local angle = math.atan(distanceVec[2],distanceVec[1])
				
				if angle < 0 then
					angle = angle * -1
				end
			
				if angle >= (10 * math.pi / 12) then
					angle = 10 * math.pi / 12
				elseif angle < 2 * math.pi / 12 and angle >= 0 then
					angle = 2 * math.pi / 12
				end
				
				collidedWith:knockBack(angle, self.knockBackForce)	
			end
		end
	end
end
