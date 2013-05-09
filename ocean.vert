#version 120
uniform float time;
varying vec4 diffuse, ambient;
varying vec3 normal, halfVector;

void main() {       
    vec4 pos = gl_Vertex;
    normal = gl_Normal;
    halfVector = gl_LightSource[0].halfVector.xyz;
    // Compute the base diffuse color (not the intensity)
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

    // Compute the ambient color
    ambient = gl_FrontMaterial.ambient *
              (gl_LightSource[0].ambient + gl_LightModel.ambient);
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
