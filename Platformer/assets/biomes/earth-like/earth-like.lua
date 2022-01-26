earthLike_wg = {}

function earthLike_wg:setProperties()
	self.generationAssets = {
		{"base","stone_m", 30,30, 10},
		{"base", "ground_m", 100,10, 10},
		{"top", true, "grass_mm", "none"},
		{"top", false, "grass_mm", "none"},
		{"caves", true, 35, 50, 5},
		{"spots", true, "bronze_mm", "stone_m", 100, 8}
	}
	
	self.premades = {
		
	
	
	
	
	}
end

newWorldGenerator("earthLike_wg")
