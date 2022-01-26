wander_be = {}

function wander_be:whileActive(agent)
	local wanderTimer = agent:getTimer("wanderTimer")
	local nextChange = agent:getInt("nextChange")
	local isWalking = agent:getBool("isWalking")
	local walkingRight = agent:getBool("walkingRight")
	
	if wanderTimer:getTime() >= nextChange then
		if isWalking then
			agent:setBool("isWalking", false)
			agent:setInt("nextChange", math.random(2000,8000))
		else
			agent:setBool("isWalking", true)
			agent:setBool("walkingRight", math.coinFlip())
			agent:setInt("nextChange", math.random(1000,2500))
		end
		wanderTimer:reset()
		
	end
	
	if isWalking then
		if walkingRight then
			agent:walkRight()
		else
			agent:walkLeft()
		end
	end

end

function wander_be:whenActivated(agent)
	agent:setTimer("wanderTimer")
	agent:setInt("nextChange", math.random(500,1500))
	agent:setBool("isWalking", math.coinFlip())
	agent:setBool("walkingRight", math.coinFlip())
end

function wander_be:whenDisactivated(agent)
	
end