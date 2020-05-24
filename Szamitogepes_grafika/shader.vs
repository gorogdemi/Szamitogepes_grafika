#version 430

/** Uniform változók. */
layout (std140, binding = 0) uniform ModelData
{
	mat4 mModelView;
	mat4 mView;
	mat4 mNormal;
	mat4 mMVP;
} sModelData;

/** Bemeneti attribútumok. */
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

/** Kimeneti attribútumok. */
out vec3 vPosVS;
out vec3 vNormalVS;

void main()
{
	
	vPosVS = vec3(sModelData.mModelView * vec4(vPos, 1));
	vNormalVS = vNormal;
	/** Kiírjuk a pozíciót. */
	gl_Position = sModelData.mMVP * vec4(vPos, 1);
}