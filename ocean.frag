varying vec4 diffuse,ambient;
varying vec3 normal,halfVector;
 
void main()
{
    float NdotL,NdotHV;
 
    // ambient term
    vec4 color = ambient;

    // compute the dot product between normal and light direction
    vec3 n = normalize(normal);
    NdotL = max(dot(n,gl_LightSource[0].position.xyz),0.0);

    // only add diffuse and specular if light is hitting from front
    if (NdotL > 0.0) {
        // diffuse term
        color += diffuse * NdotL;
	
	// specular term
        NdotHV = max(dot(n,normalize(halfVector)),0.0);
        color += gl_FrontMaterial.specular *
                gl_LightSource[0].specular *
                pow(NdotHV, gl_FrontMaterial.shininess);
    }
 
    gl_FragColor = color;
}
