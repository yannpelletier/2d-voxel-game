rain_pr = {}

function rain_pr:setProperties()
	self.texture = "projectiles/weather/rain/texture.png"
	self.size = {10,3}
	self.spritesheetSize = {3,1}
	self.sensor = true
	self.density = 0
	self.friction = 0
	self.animations = {
		{"variation1",0,0,5000},
		{"variation2",1,1,5000},
		{"variation3",2,2,5000}
	}
	
	self.speedRange = {400, 640}
end

newProjectile("rain_pr")

function rain_pr:onCreate(projectile)
	projectile:setAnimation("variation" .. math.random(1,3))

	projectile:addNum("xDirection", projectile:getDirection()[1])
	projectile:addNum("yDirection", projectile:getDirection()[2])
	projectile:addTimer("lifeTimer")
	projectile:addNum("speed", math.randomFloat(self.speedRange[1],self.speedRange[2]))
end

function rain_pr:update(projectile)
	local speed = projectile:getValue("speed")
	projectile:setVelocity(projectile:getValue("xDirection") * speed, projectile:getValue("yDirection") * speed)
	
	local dir = projectile:getDirection()
	local angle = vec2.angle(dir)
	projectile:setAngleRad(angle)
	
	if projectile:getValue("lifeTimer"):getTime() >= 10000 then
		projectile:destroy()
	end
end

function rain_pr:onContact(projectile, collidedWith, collidedType)
	if collidedType == "tile" then
		local position = projectile:getPosition()
		local raycastPoint = world.raycast(position[1],position[2] + rain_splash_ef.size[2],-90, 20)
		world.createEffect("rain_splash_ef",position[1],raycastPoint[2] + (rain_splash_ef.size[2] / 2))

		projectile:destroy()
	end
end