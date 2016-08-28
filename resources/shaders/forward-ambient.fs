#version 330

in vec2 texCoord0;

out vec4 fragColor;

uniform sampler2D sampler;
uniform vec3 ambientIntensity;

void main()
{   
	fragColor = texture2D(sampler,texCoord0.xy) * vec4(ambientIntensity, 1);
}