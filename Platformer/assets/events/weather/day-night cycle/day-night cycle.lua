dn_cycle_evnt = {}

function dn_cycle_evnt:setProperties()
	self.transitionSpeed = 45000
	self.dayDuration = 120000
	self.nightDuration = 120000
	self.nightLightColor = 70
	self.dayLightColor = 195
end

newEvent("dn_cycle_evnt")

function dn_cycle_evnt:whenActivated(event)
	local lightRising = true
	event:addBool("lightRising",lightRising)
	event:addBool("transitioning", false)
	event:addTimer("hourTimer")
	
	if lightRising then
		world.addToAmbientLightColor(self.nightLightColor,self.nightLightColor,self.nightLightColor)
	else
		world.addToAmbientLightColor(self.dayLightColor,self.dayLightColor,self.dayLightColor)
	end
end

function dn_cycle_evnt:whileActive(event)
	local lightRising = event:getValue("lightRising")
	local transitioning = event:getValue("transitioning")
	local hourTimer = event:getValue("hourTimer")
	local hourTime = hourTimer:getTime()
	
	local color = 0
	
	if transitioning then
		if hourTime >= self.transitionSpeed then
			event:setValue("transitioning", math.oppositeValue(transitioning))
			hourTimer:reset()
		end
	else
		if lightRising then
			if hourTime >= self.dayDuration then
				event:setValue("lightRising", math.oppositeValue(lightRising))
				event:setValue("transitioning", math.oppositeValue(transitioning))
				hourTimer:reset()
			end
		else
			if hourTime >= self.nightDuration then
				event:setValue("lightRising", math.oppositeValue(transitioning))
				event:setValue("transitioning", math.oppositeValue(transitioning))
				hourTimer:reset()
			end
		end
	end
	
	lightRising = event:getValue("lightRising")
	transitioning = event:getValue("transitioning")
	hourTime = hourTimer:getTime()
	
	if lightRising then
		if transitioning then
			color = math.interpolate(self.nightLightColor, self.dayLightColor, hourTime, self.transitionSpeed)
			
			world.addToAmbientLightColor(color,color,color)
		else
			color = math.seeSaw(self.dayLightColor, 255, hourTime, self.dayDuration / 2, self.dayDuration)
			
			world.addToAmbientLightColor(color,color,color)
		end
	else
		if transitioning then
			color = math.interpolate(self.dayLightColor, self.nightLightColor, hourTime, self.transitionSpeed)
			
			world.addToAmbientLightColor(color,color,color)
		else
			color = math.seeSaw(self.nightLightColor, 0, hourTime, self.nightDuration / 2, self.nightDuration)
			
			world.addToAmbientLightColor(color,color,color)
		end
	end
end

function dn_cycle_evnt:whenDisactivated(event)
	
end