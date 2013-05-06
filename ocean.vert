uniform float time;
varying vec4 diffuse, ambient;
varying vec3 normal, halfVector;

void main() {       
    vec4 pos = gl_Vertex;
    pos.y = pos.y + 0.33*sin(pos.x + time * 0.85) + 0.45*cos(pos.z + time * 0.65);
    normal = vec3(pos.x, -1.0, pos.z);
    normal.x = -0.33*cos(pos.x + time * 0.85);
    normal.y = 1.0;
    normal.z = 0.45*sin(pos.z + time * 0.65);
    halfVector = gl_LightSource[0].halfVector.xyz;
    // Compute the base diffuse color (not the intensity)
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

    // Compute the ambient color
    ambient = gl_FrontMaterial.ambient *
              (gl_LightSource[0].ambient + gl_LightModel.ambient);
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
