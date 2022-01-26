function math.coinFlip()
	local value = math.random(1,2)
	
	if value == 1 then
		return false
	else
		return true
	end
end

function math.round(value)
	local decimals = value - math.floor(value)
	
	if decimals < 0.5 then
		return math.floor(value)
	else
		return math.ceil(value)
	end
end

function math.oppositeValue(boolValue)
	if boolValue then
		return false
	else
		return true
	end
end

function math.toDegree(angleRad)
	return angleRad * 360 / (2 * math.pi)
end

function math.toRadian(angleDeg)
	return angleDeg * (2 * math.pi) / 360
end

function math.interpolate(beginValue, endValue, currentTime, endTime)
	local range = endValue - beginValue
	local timeScale = currentTime / endTime
		
	local interpolatedValue = beginValue + (range * timeScale)
	
	if currentTime < 0 then
		interpolatedValue = beginValue
	elseif currentTime > endTime then
		interpolatedValue = endValue
	end
	
	return interpolatedValue
end

function math.angleBetweenDeg(angle1, angle2)
	local angle1Rad = math.toRadian(angle1)
	local angle2Rad = math.toRadian(angle2)
	local vector1 = {}
	vector1[1] = math.cos(angle1Rad)
	vector1[2] = math.sin(angle1Rad)
	
	local vector2 = {}
	vector2[1] = math.cos(angle2Rad)
	vector2[2] = math.sin(angle2Rad)
		
	local dotProduct = vec2.dot(vector1,vector2)
	local normProduct = vec2.mag(vector1) * vec2.mag(vector2)
		
	return math.toDegree(math.acos(dotProduct / normProduct))
end

function math.seeSaw(beginValue, endValue, currentTime, middleTime, endTime)
	local interpolatedValue = 0

	if currentTime <= middleTime then
		interpolatedValue = math.interpolate(beginValue, endValue, currentTime, middleTime)
	elseif currentTime > middleTime and currentTime <= endTime then
		interpolatedValue = math.interpolate(endValue, beginValue, currentTime - middleTime, endTime - middleTime)
	end
	
	if currentTime < 0 then
		interpolatedValue = beginValue
	elseif currentTime > endTime then
		interpolatedValue = beginValue
	end
		
	return interpolatedValue
end

function math.randomFloat(lower, greater)
    return lower + math.random()  * (greater - lower);
end