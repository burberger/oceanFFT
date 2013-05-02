#version 120
uniform float time;
void main()
{       
    vec4 pos = gl_Vertex;
    pos.y = pos.y + 0.33*sin(pos.x * time) + 0.25*sin(pos.x * time * 0.5) + 0.17*cos(pos.z * time);
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
