#version 430

/** Bemeneti attribútumok. */
in vec3 vPosVS;
in vec3 vNormalVS;
in vec3 vNormalVS2;

/** Kimeneti puffer. */
out vec4 outColor;

in vec3 vColorVS;

layout(location=3) uniform vec3 color;

void main()
{
	vec3 lightDir=normalize(vec3(100.0,100.0,100.0));
	float diff=max(dot(vNormalVS,lightDir),0.0);
	float spec = pow(max(dot(vNormalVS,lightDir), 0.0), 16)*5;
	vec3 diffuse=diff*vec3(1.0,1.0,1.0);
	vec3 result = (diffuse+vec3(0.3,0.3,0.3)+spec) * vColorVS;
	
	if(vColorVS[0]>0.5&&vColorVS[1]<0.7&&vColorVS[2]<0.7) 
	{
		outColor = vec4(vColorVS,1.0);	
	}
	else
	{
		outColor = vec4(result,1.0);	
	}
	outColor = vec4(result,1.0);
}
/** * vec3(0.2,0.2,1.0)*/