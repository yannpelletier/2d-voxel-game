flashlight_ui = {}

function flashlight_ui:setProperties()
	--Used in the game
	self.showcaseName = "Flashlight"
	self.texture = "items/flashlight/texture.png"
	self.icon = "items/flashlight/icon.png"
	self.type = "Utility"
	self.itemSize = {25,10}
	self.offset = {0,0}
	self.angleOffset = 0
	self.spritesheetSize = {1,2}
	self.animations = {
		{"default",1,1,5000},
		{"closed",0,0,5000}
	}
	self.twoHanded = false

end

newUsableItem("flashlight_ui")

function flashlight_ui.turnOff(heldItem)
	local light = heldItem:getLight()
	light:setRadius(0)
	heldItem:setAnimation("closed")
	heldItem:setValue("on", false)
end

function flashlight_ui.turnOn(heldItem)
	local light = heldItem:getLight()
	light:setRadius(20)
	heldItem:setAnimation("default")
	heldItem:setValue("on", true)
end

--Called when the item gets equipped
function flashlight_ui:onEquip(heldItem, mousePos)
	heldItem:addLight()
	local light = heldItem:getLight()
	light:setRadius(20)
	light:setIntensity(7)
	light:setColor(150,255,255,255)
	light:setConeAngle(25)

	heldItem:addBool("on", true)
end

--Called when the item gets unequipped
function flashlight_ui:onUnequip(heldItem, mousePos)

end

--Called while the item is equipped
function flashlight_ui:whileEquipped(heldItem, mousePos)
	local light = heldItem:getLight()
	
	local dirVec = heldItem:getDirection()

	light:setDirection(dirVec[1], dirVec[2])
	
	local on = heldItem:getValue("on")
	
	if input.keyPressed(heldItem:getUseButton()) then
			if on then
				self.turnOff(heldItem)
			else 
				self.turnOn(heldItem)
			end
	end

end
