uniform float time;
void main() {       
    vec4 pos = gl_Vertex;
    pos.y = pos.y + 0.33*sin(pos.x + time * 0.85) + 0.45*cos(pos.z + time * 0.65);
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
