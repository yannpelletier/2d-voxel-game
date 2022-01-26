bronze_ui = {}

function bronze_ui:setProperties()
	--Used in the game
	self.showcaseName = "Bronze ore"
	self.texture = "resources/minerals/bronze/texture.png"
	self.icon = "resources/minerals/bronze/icon.png"
	self.type = "Resource"
	self.itemSize = {25,10}
	self.offset = {0,0}
	self.spritesheetSize = {1,1}
	self.animations = {
		{"default",0,0,5000}
	}
	self.twoHanded = true

end

newUsableItem("bronze_ui")

--Called when the item gets equipped
function bronze_ui:onEquip(heldItem, mousePos)

end

--Called when the item gets unequipped
function bronze_ui:onUnequip(heldItem, mousePos)

end

--Called while the item is equipped
function bronze_ui:whileEquipped(heldItem, mousePos)

end
