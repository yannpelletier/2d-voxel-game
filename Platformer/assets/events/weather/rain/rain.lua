rain_evnt = {}

function rain_evnt:setProperties()
	self.lowDropAmount = 5
	self.highDropAmount = 50
	self.soundDuration = 4000
	self.fadeSpeed = 750
end

newEvent("rain_evnt")

function rain_evnt:whenActivated(event)
	event:addTimer("rainLoopTimer")
	event:addTimer("volumeTimer")
	event:addBool("wasHearingTheRain", false)	
	event:addInt("currentVolume", 0)
	
	local duration = event:getDuration()
	local loopTime = math.ceil(duration / self.soundDuration) --Divide the duration by the duration of the sound to know how many times to loop
	
	sound.playSFX("assets/events/weather/rain/intense rain.wav", loopTime, channels.rain)
	sound.setChannelVolume(channels.rain,0)
end

function rain_evnt:whileActive(event)
	local cameraPosition = world.getCameraPosition()
	local cameraDimensions = world.getCameraDimensions()
	
	local rainLoopTimer = event:getValue("rainLoopTimer")
	local rainLoopTime = rainLoopTimer:getTime()
	
	local volumeTimer = event:getValue("volumeTimer")
	
	local occuranceTime = event:getOccuranceTime()
	local duration = event:getDuration()
	
	local seeingTheRain = false
	local hearingTheRain = false
	local wasHearingTheRain = event:getValue("wasHearingTheRain")
		
	local volume = 0
	local currentVolume = event:getValue("currentVolume")
		
	--Check whether we should hear and see the rain
	for i = 0,100,1 do
		local raycastX = (cameraPosition[1] - cameraDimensions[1] / 2) + ((cameraDimensions[1] / 100) * i)
		local raycastPoint = world.raycast(raycastX,150000,-90, 150000)
		if math.abs(raycastPoint[2] - cameraPosition[2]) <= cameraDimensions[2] / 2 or cameraPosition[2] - cameraDimensions[2] / 2 >= raycastPoint[2]  then
			seeingTheRain = true
			volume = volume + 1
			if volume == 10 then
				hearingTheRain = true
			end
		end
	end
				
	if wasHearingTheRain ~= hearingTheRain then
		event:setValue("wasHearingTheRain", math.oppositeValue(wasHearingTheRain))
		volumeTimer:reset()
	end
	
	local volumeTime = volumeTimer:getTime()
			
	--Handle the volume of the rain sound
	if hearingTheRain then
		if occuranceTime < 2000 then
			sound.setChannelVolume(channels.rain, math.round(math.interpolate(0,volume * 3, occuranceTime, 2000)))
		elseif occuranceTime > duration - 2000 then
			sound.setChannelVolume(channels.rain, math.round(math.interpolate(volume * 3,0, occuranceTime - (duration - 2000), 2000)))
		else
			sound.setChannelVolume(channels.rain, math.round(math.interpolate(0,volume * 3, volumeTime, self.fadeSpeed)))
		end
	else
		sound.setChannelVolume(channels.rain, math.round(math.interpolate(currentVolume,0, volumeTime, self.fadeSpeed)))
	end
	
	if hearingTheRain then
		event:setValue("currentVolume", volume * 3)
	end
	
	--Produce the rain drops
	if seeingTheRain then
		if occuranceTime <= 3000 or occuranceTime >= duration - 3000 then
			if rainLoopTime >= 150 then
				rainLoopTimer:reset()
				for i = 0, self.lowDropAmount, 1 do
					world.createProjectile("rain_pr",cameraPosition[1] + math.randomFloat(-cameraDimensions[1] / 2 - 500, cameraDimensions[1] / 2 + 500), cameraPosition[2] + cameraDimensions[2] + 500 + math.randomFloat(-10,10),0,-1)
				end
			end
		else
			if rainLoopTime >= 150 then
				rainLoopTimer:reset()
				for i = 0, self.highDropAmount, 1 do
					world.createProjectile("rain_pr",cameraPosition[1] + math.randomFloat(-cameraDimensions[1] / 2 - 500, cameraDimensions[1] / 2 + 500), cameraPosition[2] + cameraDimensions[2] + 500 + math.randomFloat(-10,10),0,-1)
				end
			end
		end	
	end
end

function rain_evnt:whenDisactivated(event)
	sound.setChannelVolume(channels.rain,0)
end