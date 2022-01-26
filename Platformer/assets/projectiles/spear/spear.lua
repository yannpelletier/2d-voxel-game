spear_pr = {}

function spear_pr:setProperties()
	self.texture = "projectiles/spear/texture.png"
	self.size = {50,10}
	self.spritesheetSize = {1,1}
	self.sensor = true
	self.density = 0
	self.friction = 0
	self.animations = {
		{"default",0,0,5000}
	}
end

newProjectile("spear_pr")

function spear_pr:onCreate(projectile)		

end

function spear_pr:update(projectile)
	local dir = projectile:getDirection()
	local angle = vec2.angle(dir)
	
	projectile:setAngleRad(angle)
end

function spear_pr:onContact(projectile, collidedWith, collidedType)
	if collidedType == "agent" then
		collidedWith:damage(50)
	end

	if collidedType ~= "thrower" then
		projectile:destroy("throwable_spear_ui")
	end
end