thunderstorm_evnt = {}

function thunderstorm_evnt:setProperties()
	self.thunderTimeRange = {7000,45000}
	self.thunderSpeed = 75
	self.thunderSounds = {"assets/events/weather/thunderstorms/thunder1.wav", "assets/events/weather/thunderstorms/thunder2.wav", "assets/events/weather/thunderstorms/thunder3.wav"}
	self.lightDimScale = -50
	self.lethalThunderFrequency = 20
end

newEvent("thunderstorm_evnt")

function thunderstorm_evnt:whenActivated(event)
	event:addInt("nextThunder", math.random(self.thunderTimeRange[1],self.thunderTimeRange[2]))
	event:addBool("startedThunder", false)
	event:addTimer("nextThunderTimer")
	event:addTimer("thunderProductionTimer")
end

function thunderstorm_evnt:produceThunder(event)
	local thunderProductionTimer = event:getValue("thunderProductionTimer")
	local thunderProductionTime = thunderProductionTimer:getTime()

	local nextThunderTimer = event:getValue("nextThunderTimer")
	
	if thunderProductionTime < self.thunderSpeed then
		local colorToSetR = math.seeSaw(0, 150, thunderProductionTime, self.thunderSpeed / 2, self.thunderSpeed)
		local colorToSetG = math.seeSaw(0, 150, thunderProductionTime, self.thunderSpeed / 2, self.thunderSpeed)
		local colorToSetB = math.seeSaw(0, 150, thunderProductionTime, self.thunderSpeed / 2, self.thunderSpeed)
		
		world.addToAmbientLightColor(colorToSetR,colorToSetG,colorToSetB)
	elseif thunderProductionTime > self.thunderSpeed and thunderProductionTime <= self.thunderSpeed * 2 then
		local colorToSetR = math.seeSaw(0, 150, thunderProductionTime - self.thunderSpeed, self.thunderSpeed / 2, self.thunderSpeed)
		local colorToSetG = math.seeSaw(0, 150, thunderProductionTime - self.thunderSpeed, self.thunderSpeed / 2, self.thunderSpeed)
		local colorToSetB = math.seeSaw(0, 150, thunderProductionTime - self.thunderSpeed, self.thunderSpeed / 2, self.thunderSpeed)
		
		world.addToAmbientLightColor(colorToSetR,colorToSetG,colorToSetB)
	else
		nextThunderTimer:reset()
		event:setValue("startedThunder", false)
		event:setValue("nextThunder", math.random(self.thunderTimeRange[1], self.thunderTimeRange[2]))
	end
	
end

function thunderstorm_evnt:whileActive(event)
	local cameraPosition = world.getCameraPosition()
	local cameraDimensions = world.getCameraDimensions()

	local nextThunder = event:getValue("nextThunder")
	local nextThunderTimer = event:getValue("nextThunderTimer")
	local nextThunderTime = nextThunderTimer:getTime()
		
	local startedThunder = event:getValue("startedThunder")
	
	local thunderProductionTimer = event:getValue("thunderProductionTimer")
		
	if nextThunderTime >= nextThunder then
		if startedThunder == false then
			local playThunderSound = false
			for i = 0,100,1 do
				local raycastX = (cameraPosition[1] - cameraDimensions[1] / 2) + ((cameraDimensions[1] / 100) * i)
				local raycastPoint = world.raycast(raycastX,150000,-90, 150000)
				if math.abs(raycastPoint[2] - cameraPosition[2]) <= cameraDimensions[2] / 2 or cameraPosition[2] - cameraDimensions[2] / 2 >= raycastPoint[2]  then
					playThunderSound = true
					break
				end
			end
			if playThunderSound then
				if math.random(1,self.lethalThunderFrequency) == self.lethalThunderFrequency then	--Only has a 10% chance of creating a lethal thunderbolt near the player
					local thunderX = cameraPosition[1] + math.randomFloat(-cameraDimensions[1] / 2, cameraDimensions[1] / 2)
					local raycastedPoint = world.raycast(thunderX, cameraPosition[2] + 2000, -90, 4000)
				
					world.createEffect("thunderstorm_ef",thunderX, raycastedPoint[2] + 1500)
					sound.playSFX("assets/events/weather/thunderstorms/thundernear.wav", loopTime)
				else
					sound.playSFX(self.thunderSounds[math.random(1,3)], loopTime)
				end
			end
			
			event:setValue("startedThunder", true)
			thunderProductionTimer:reset()
		end
		
		self:produceThunder(event)
	end
	
	local occuranceTime = event:getOccuranceTime()
	local duration = event:getDuration()
	
	if occuranceTime <= duration * 0.1 then
		local colorToSubstract = math.interpolate(0, self.lightDimScale, occuranceTime, duration * 0.1)
		world.addToAmbientLightColor(colorToSubstract,colorToSubstract,colorToSubstract)
	elseif occuranceTime >= duration * 0.9 then
		local colorToSubstract = math.interpolate(self.lightDimScale, 0, occuranceTime - duration * 0.9, duration * 0.1)
		world.addToAmbientLightColor(colorToSubstract,colorToSubstract,colorToSubstract)
	else
		world.addToAmbientLightColor(self.lightDimScale,self.lightDimScale,self.lightDimScale)
	end
end

function thunderstorm_evnt:whenDisactivated(event)
	
end