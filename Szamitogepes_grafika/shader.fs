#version 430

/** Bemeneti attribútumok. */
in vec3 vPosVS;
in vec3 vNormalVS;

/** Kimeneti puffer. */
out vec4 outColor;

void main()
{
	outColor = vec4(normalize(vNormalVS+vec3(1.0,1.0,1.0)),1.0);
}