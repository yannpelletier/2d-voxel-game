red_flare_pr = {}

function red_flare_pr:setProperties()
	self.texture = "projectiles/flares/red flare/texture.png"
	self.size = {20,5}
	self.spritesheetSize = {1,1}
	self.sensor = false
	self.density = 10
	self.friction = 5
	self.animations = {
		{"default",0,0,5000}
	}
	
	self.flickerSpeed = 175
	self.minIntensity = 5
	self.maxIntensity = 7
	self.distanceUntilDeaf = 1000
end

newProjectile("red_flare_pr")

function red_flare_pr:onCreate(projectile)		
	projectile:addLight()
	local light = projectile:getLight()
	light:setColor(255,0,0)
	light:setIntensity(10)
	light:setRadius(10)
	
	projectile:setEmitter("flare_smoke_pe")
	local emitter = projectile:getEmitter()
	emitter:addEmittedParticle("flare_smoke_1", {255,0,0,0})
	emitter:addEmittedParticle("flare_smoke_2", {255,0,0,0})
	
	local dir = projectile:getDirection()
	local angle = vec2.angle(dir)
	
	projectile:setAngleRad(angle)
	
	projectile:addTimer("lifeTimer")
	projectile:addTimer("flickerTimer")
	
	if projectile:getDirection()[1] > 0 then
		projectile:setAngularVelocity(-15)
	else
		projectile:setAngularVelocity(15)
	end
	
	sound.playSFX("assets/projectiles/flares/activation.wav")
	sound.playSFX("assets/projectiles/flares/lit.wav",0,channels.flare)
	
	projectile:addInt("previousVolume", 0)
	local currentChannelVolume = sound.getChannelVolume(channels.flare)
	if currentChannelVolume == 100 then
		sound.setChannelVolume(channels.flare, 0)
	end
end

function red_flare_pr:update(projectile)
	--Create a flickering effect
	local light = projectile:getLight()
	local flickerTimer = projectile:getValue("flickerTimer")
	local flickerTime = flickerTimer:getTime()
	
	light:setIntensity(math.seeSaw(self.minIntensity,self.maxIntensity, flickerTime, self.flickerSpeed / 2, self.flickerSpeed))
	
	if flickerTime > self.flickerSpeed then
		flickerTimer:reset()
	end
	
	--Set the flare volume
	local cameraPosition = world.getCameraPosition()
	local projectilePosition = projectile:getPosition()
	local distanceVec = {}
	distanceVec[1] = projectilePosition[1] - cameraPosition[1]
	distanceVec[2] = projectilePosition[2] - cameraPosition[2]

	local distance = vec2.mag(distanceVec)
	local calculatedVolume = math.round(((self.distanceUntilDeaf - distance) / self.distanceUntilDeaf) * 100)
	local previousVolume = projectile:getValue("previousVolume")

	local currentChannelVolume = sound.getChannelVolume(channels.flare)
	
	if calculatedVolume < 0 then
		calculatedVolume = 0
		previousVolume = 0
	end
		
	projectile:setValue("previousVolume", calculatedVolume)
	
	
	sound.setChannelVolume(channels.flare, currentChannelVolume + (calculatedVolume - previousVolume))

	--Destroy after 20 seconds
	if projectile:getValue("lifeTimer"):getTime() >= 20000 then
		sound.setChannelVolume(channels.flare, currentChannelVolume - previousVolume)
		projectile:destroy()
	end
	
end

function red_flare_pr:onContact(projectile, collidedWith, collidedType)

end