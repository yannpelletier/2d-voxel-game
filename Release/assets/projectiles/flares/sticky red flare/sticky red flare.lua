sticky_red_flare_pr = {}

function sticky_red_flare_pr:setProperties()
	self.texture = "projectiles/flares/sticky red flare/texture.png"
	self.size = {20,5}
	self.spritesheetSize = {1,1}
	self.sensor = false
	self.density = 10
	self.friction = 0.1
	self.animations = {
		{"default",0,0,5000}
	}
	
	self.flickerSpeed = 175
	self.minIntensity = 5
	self.maxIntensity = 7
	self.distanceUntilDeaf = 1000
end

newProjectile("sticky_red_flare_pr")

function sticky_red_flare_pr:onCreate(projectile)		
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
	projectile:addBool("stuck", false)
	
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

function sticky_red_flare_pr:update(projectile)
	local velocity = projectile:getVelocity()
	local angularVelocity = projectile:getAngularVelocity()
	
	local light = projectile:getLight()
	local flickerTimer = projectile:getValue("flickerTimer")
	local flickerTime = flickerTimer:getTime()
		
	--Create a flickering effect
	light:setIntensity(math.seeSaw(self.minIntensity,self.maxIntensity, flickerTime, self.flickerSpeed / 2, self.flickerSpeed))
	
	if flickerTime > self.flickerSpeed then
		flickerTimer:reset()
	end
	
	--Stick the flare to the wall
	if projectile:getValue("stuck") == true then
		projectile:setVelocity(0,0)
		projectile:setAngularVelocity(0)
		projectile:setGravityScale(0)
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

function sticky_red_flare_pr:onContact(projectile, collidedWith, collidedType)
	if collidedType == "tile" then
		projectile:setValue("stuck", true)
	end
end