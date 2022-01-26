handgun_ui = {}

function handgun_ui:setProperties()
	--Used in the game
	self.showcaseName = "Handgun"
	self.texture = "items/guns/handgun/texture.png"
	self.icon = "items/guns/handgun/icon.png"
	self.type = "Gun"
	self.itemSize = {25,17}
	self.drawScale = {1,1}
	self.offset = {5,7}
	self.angleOffset = 0
	self.armAngleOffset = 0
	self.spritesheetSize = {1,1}
	self.animations = {
		{"default",0,0,5000}
	}
	self.twoHanded = false 
	
	self.flareIntensity = 8
	self.flareDelay = 100
	self.shootDelay = 250
end

newUsableItem("handgun_ui")

--Called when the item gets equipped
function handgun_ui:onEquip(heldItem, mousePos)
	heldItem:addLight()
	local light = heldItem:getLight()
	light:setRadius(8)
	light:setIntensity(0)
	light:setColor(255,165,0,255)
	light:setConeAngle(25)

	
	heldItem:addTimer("flareTimer")
	heldItem:addBool("shotOnce", false)
end

--Called when the item gets unequipped
function handgun_ui:onUnequip(heldItem, mousePos)

end

--Called while the item is equipped
function handgun_ui:whileEquipped(heldItem, mousePos)
	local positionVec = heldItem:getPosition()
	positionVec[1] = positionVec[1] + heldItem:calculateOffset(-5,6)[1]
	positionVec[2] = positionVec[2] + heldItem:calculateOffset(-5,6)[2]
	local directionVec = {}
	directionVec[1] = mousePos[1] - positionVec[1] 
	directionVec[2] = mousePos[2] - positionVec[2]
	directionVec = vec2.norm(directionVec)
	
	local heldItemAngle = vec2.angleDeg(heldItem:getDirection())
	local calculatedAngle = vec2.angleDeg(directionVec)
	
	while heldItemAngle < 0 do
		heldItemAngle = heldItemAngle + 360
	end
	while heldItemAngle >= 360 do
		heldItemAngle = heldItemAngle - 360
	end
	while calculatedAngle < 0 do
		calculatedAngle = calculatedAngle + 360
	end
	while calculatedAngle >= 360 do
		calculatedAngle = calculatedAngle - 360
	end
	
	local flareTimer = heldItem:getValue("flareTimer")
	local flareTime = flareTimer:getTime()
	local light = heldItem:getLight()
		
	--Otherwise, if the cursor is behind the bullet goes backwards
	if math.angleBetweenDeg(calculatedAngle, heldItemAngle) <= 40 then
	else
		directionVec[1] = math.cos(heldItemAngle * (2 * math.pi) / 360)
		directionVec[2] = math.sin(heldItemAngle * (2 * math.pi) / 360)
	end
	
	if heldItem:getArmAnimationName() == "holding" then
		if input.isKeyDown(heldItem:getUseButton()) then
			heldItem:setValue("shotOnce", true)
			sound.playSFX("assets/items/guns/handgun/gunshot.wav", 0)
			heldItem:createProjectile("bullet_pr",-5,6,directionVec[1], directionVec[2])
			light:setDirection(directionVec[1],directionVec[2])
			heldItem:playAnimation("shoot", self.shootDelay)
			flareTimer:reset()
		end
	end
		
	--Creates a flare when shot
	if heldItem:getValue("shotOnce") == true then
		light:setIntensity(math.seeSaw(0,self.flareIntensity,flareTime, self.flareDelay / 2, self.flareDelay))
	end
	
end

function handgun_ui:onContact(heldItem, collidedWith, collidedType)

end
