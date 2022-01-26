jumpOverObstacles_be = {}

function jumpOverObstacles_be:whileActive(agent)
	local center = agent:getPosition()
	local velocity = agent:getVelocity()
	local dimensions = agent:getDimensions()
	
	local bottomY = center[2] - (dimensions[2] / 2)
	
	local tileX
	
	if agent:isWalkingLeft() then
		tileX = math.floor((center[1]) / world.tileSize) - 1
	elseif agent:isWalkingRight() then
		tileX = math.ceil((center[1]) / world.tileSize) + 1
	end
	
	local agentTileYBottom = math.floor( ( center[2] - (dimensions[2] / 2) )  / world.tileSize) + 1
	local agentTileYTop = math.ceil( ( center[2] + (dimensions[2] / 2) )  / world.tileSize)
	
	local hasObstacle = false
	
	for i = agentTileYBottom, agentTileYTop do
		if world.getTile(tileX, i):isSolid() then
			hasObstacle = true
		end
	end
	
	if hasObstacle then	
		local wallTopTileY = agentTileYBottom
		local observedTileY = agentTileYBottom
		local agentHeightInTiles = math.ceil(dimensions[2] / world.tileSize)
		
		local emptyTileNumber = 0
		
		while emptyTileNumber ~= agentHeightInTiles do -- Find the height of the wall
			if world.getTile(tileX, observedTileY):isSolid() then
				wallTopTileY  = observedTileY
				emptyTileNumber = 0
			else
				emptyTileNumber = emptyTileNumber + 1
			end
			
			observedTileY = observedTileY + 1
		end
		
		
		if wallTopTileY - agentTileYBottom <= math.floor(agent:getJumpForce() / 3) then --Can jump the wall
			agent:jump()
		end
	end
end