 #version 140
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color;
uniform float intensity;
uniform float coneAngle; 
uniform vec2 lightDirection;

void main() {
	float lightAngle = coneAngle;
	vec2 normalizedLightDirection = normalize(lightDirection);
	if(lightAngle == 180){
		float distance = length(fragmentUV);
		color.rgb = fragmentColor.rgb;
		color.a =  1 * (1.0 -(pow(distance, 2)));
		color = color * intensity;
		return;
	}
	else{
		vec2 uvVec = normalize(fragmentUV);
		float lightToSurfaceAngle = degrees(acos(dot(uvVec, normalizedLightDirection)));
		
		if(lightToSurfaceAngle <= lightAngle){
			float distance = length(fragmentUV);
			color.rgb = fragmentColor.rgb;
			color.a =  fragmentColor.a * (1.0 -(pow(distance, 2)));
			color = color * intensity;
			return;
		}else{
			color = vec4(0,0,0,0);
			return;
		}
	}

}