bullet_pr = {}

function bullet_pr:setProperties()
	self.texture = "projectiles/bullet/texture.png"
	self.size = {15,7.5}
	self.spritesheetSize = {1,1}
	self.sensor = true
	self.density = 0
	self.friction = 0
	self.animations = {
		{"default",0,0,5000}
	}
	
	self.speed = 1600
end

newProjectile("bullet_pr")

function bullet_pr:onCreate(projectile)
	projectile:addNum("xDirection", projectile:getDirection()[1])
	projectile:addNum("yDirection", projectile:getDirection()[2])
	projectile:addTimer("lifeTimer")
end

function bullet_pr:update(projectile)
	projectile:setVelocity(projectile:getValue("xDirection") * self.speed, projectile:getValue("yDirection") * self.speed)
	
	local dir = projectile:getDirection()
	local angle = vec2.angle(dir)
	projectile:setAngleRad(angle)
	
	if projectile:getValue("lifeTimer"):getTime() >= 1000 then
		projectile:destroy()
	end
end

function bullet_pr:onContact(projectile, collidedWith, collidedType)
	if collidedType == "agent" then
		collidedWith:damage(50)
	end

	if collidedType ~= "thrower" then
		projectile:destroy()
	end
end