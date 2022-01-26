skeleton_hu = {}

function skeleton_hu:setProperties()
	self.scale = 1
	self.xAcceleration = 200
	self.maxXSpeed = 2
	self.jumpForce = 15
end

newNPC("skeleton_hu")

function skeleton_hu:onSpawn(agent)
	agent:enableBehaviour("wander_be")
	agent:enableBehaviour("jumpOverObstacles_be")
end

function skeleton_hu:update()
	
end