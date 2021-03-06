#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 worldPos0;

out vec4 fragColor;

struct BaseLight
{
	vec3 color;
	float intensity;
};

struct Attenuation
{
	float constant;
	float linear;
	float exponent;
};

struct PointLight
{
	BaseLight base;
	Attenuation attenuation;
	vec3 position;
	float range;
};
 
uniform sampler2D diffuse;

uniform PointLight pointLight;

//specular
uniform vec3 eyePos;
uniform float specularIntensity;
uniform float specularPower;

vec4 calcLight(BaseLight base, vec3 direction, vec3 normal)
{
	//attenuation
	float diffuseFactor = dot(normal, -direction);
	
	vec4 diffuseColor = vec4(0,0,0,0); 
	
	if(diffuseFactor > 0)
	{
		diffuseColor = vec4(base.color, 1.0) * base.intensity * diffuseFactor;
		
	}
	
	return diffuseColor;
}

vec4 calcSpecularLight(BaseLight base, vec3 direction, vec3 normal, vec2 texCoord)
{
		//specular
		vec4 specularColor = vec4(0,0,0,0);
		vec3 directionToEye = normalize(eyePos - worldPos0);
		vec3 reflectDirection = normalize(reflect(direction, normal));
		
		float specularFactor = dot(directionToEye,reflectDirection);
		specularFactor = pow(specularFactor, specularPower);
		

		
		if(specularFactor > 0)
		{
			specularColor = vec4(base.color, 1.0) * specularIntensity * specularFactor;
			
			//if(useSpecularTexture)
				//specularColor *= texture2D(SpecularMapSampler,texCoord.xy).r;
			
		
		}
		
		return specularColor;
}


vec4 calcPointLight(PointLight pointLight, vec3 normal)
{
	vec3 lightDirection = worldPos0 - pointLight.position;
	float distanceToPoint = length(lightDirection);
	
	if(distanceToPoint > pointLight.range)
		return vec4(0,0,0,0);
	
	lightDirection = normalize(lightDirection);
	
	vec4 color = calcLight(pointLight.base, lightDirection, normal);
	float attenuation = pointLight.attenuation.constant +
						pointLight.attenuation.linear * distanceToPoint + 
						pointLight.attenuation.exponent * distanceToPoint * distanceToPoint
						+ 0.0001;
						
	//0.0001 to make sure division is never by 0
	return color / attenuation;					
}

void main()
{   
	vec4 totalLight = vec4(0,0,0, 1);
	vec4 color = texture2D(diffuse,texCoord0.xy);
	
	vec4 specularColor = vec4(0,0,0,0);
	vec3 normal = normalize(normal0);

	//specularColor = calcSpecularLight(directionalLight.base, -directionalLight.direction, normal, texCoord0);
	
	totalLight += calcPointLight(pointLight, normal);
	fragColor =  color * totalLight;
}
