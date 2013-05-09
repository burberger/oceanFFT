uniform float PI = 3.14159265358979323846264;
uniform float g = 9.81;
uniform float A = 1.2;
uniform int N = 8;
uniform float time, L;
uniform vec2 w;
varying vec4 diffuse, ambient;
varying vec3 normal, halfVector;

/*struct height_norm {*/
    /*vec2 height;*/
    /*vec3 normal;*/
/*};*/

/*// Perlin noise generator code, taken from https://github.com/ashima/webgl-noise*/
/*// Author : Ian McEwan, Ashima Arts*/
/*vec3 mod289(vec3 x) {*/
  /*return x - floor(x * (1.0 / 289.0)) * 289.0;*/
/*}*/

/*vec2 mod289(vec2 x) {*/
  /*return x - floor(x * (1.0 / 289.0)) * 289.0;*/
/*}*/

/*vec3 permute(vec3 x) {*/
  /*return mod289(((x*34.0)+1.0)*x);*/
/*}*/

/*float snoise(vec2 v)*/
  /*{*/
  /*const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0*/
                      /*0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)*/
                     /*-0.577350269189626,  // -1.0 + 2.0 * C.x*/
                      /*0.024390243902439); // 1.0 / 41.0*/
/*// First corner*/
  /*vec2 i  = floor(v + dot(v, C.yy) );*/
  /*vec2 x0 = v -   i + dot(i, C.xx);*/

/*// Other corners*/
  /*vec2 i1;*/
  /*//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0*/
  /*//i1.y = 1.0 - i1.x;*/
  /*i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);*/
  /*// x0 = x0 - 0.0 + 0.0 * C.xx ;*/
  /*// x1 = x0 - i1 + 1.0 * C.xx ;*/
  /*// x2 = x0 - 1.0 + 2.0 * C.xx ;*/
  /*vec4 x12 = x0.xyxy + C.xxzz;*/
  /*x12.xy -= i1;*/

/*// Permutations*/
  /*i = mod289(i); // Avoid truncation effects in permutation*/
  /*vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))*/
		/*+ i.x + vec3(0.0, i1.x, 1.0 ));*/

  /*vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);*/
  /*m = m*m ;*/
  /*m = m*m ;*/

/*// Gradients: 41 points uniformly over a line, mapped onto a diamond.*/
/*// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)*/

  /*vec3 x = 2.0 * fract(p * C.www) - 1.0;*/
  /*vec3 h = abs(x) - 0.5;*/
  /*vec3 ox = floor(x + 0.5);*/
  /*vec3 a0 = x - ox;*/

/*// Normalise gradients implicitly by scaling m*/
/*// Approximation of: m *= inversesqrt( a0*a0 + h*h );*/
  /*m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );*/

/*// Compute final noise value at P*/
  /*vec3 g;*/
  /*g.x  = a0.x  * x0.x  + h.x  * x0.y;*/
  /*g.yz = a0.yz * x12.xz + h.yz * x12.yw;*/
  /*return 130.0 * dot(m, g);*/
/*}*/
/*// End noise code*/
/*// -----------------------------------------------------------*/

/*// Complex number functions*/
/*vec2 conj(vec2 cnum) {*/
    /*return vec2(cnum.x, -cnum.y);*/
/*}*/

/*vec2 cmul(vec2 a, vec2 b) {*/
    /*return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);*/
/*}*/

/*float dispersion(int n, int m) {*/
    /*float w0 = 2.0 * PI / 200.0f;*/
    /*float kx = 2.0 * PI * n / L;*/
    /*float kz = 2.0 * PI * m / L;*/
    /*return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w0) * w0;*/
/*}*/

/*float phillips(int n, int m) {*/
    /*vec2 k = vec2(2 * PI * n / L, 2 * PI * m / L);*/
    /*float k_len = length(k);*/

    /*if (k_len < 0.000001) {*/
        /*return 0.0;*/
    /*}*/

    /*float k_len2 = k_len * k_len;*/
    /*float k_len4 = k_len2 * k_len2;*/

    /*float kw = dot(normalize(k), normalize(w));*/
    /*kw = kw * kw;*/

    /*float w_len = length(w);*/
    /*float Lsq = w_len * w_len / g;*/
    /*Lsq = Lsq * Lsq;*/

    /*float damping = 0.001;*/
    /*float l2 = Lsq * damping * damping;*/

    /*return A * exp(-1.0f / (k_len2 * Lsq)) / k_len4 * kw * exp(-k_len2 * l2);*/
/*}*/

/*vec2 ht_0(int n, int m) {*/
    /*float n1 = snoise(vec2(n + 17, m - 23));*/
    /*float n2 = snoise(vec2(m - 0.3*time, n + 0.4*time));*/
    /*vec2 r = vec2(n1, n2);*/
    /*return r * sqrt(phillips(n, m) / 2.0);*/
/*}*/

/*vec2 ht(float t, int n, int m) {*/
    /*vec2 ht0 = ht_0(n, m);*/
    /*vec2 ht0conj = conj(ht_0(-n, -m));*/

    /*float omegat = dispersion(n, m) * t;*/
    /*float real = cos(omegat);*/
    /*float cmp = sin(omegat);*/

    /*vec2 c0 = vec2(real, cmp);*/
    /*vec2 c1 = vec2(real, -cmp);*/
    /*return cmul(ht0, c0) + cmul(ht0conj, c1);*/
/*}*/

/*height_norm heightAndNormal(vec2 x, float t) {*/
    /*vec2 height = vec2(0.0, 0.0);*/
    /*vec3 normal = vec3(0.0, 0.0, 0.0);*/

    /*float kx, kz, kDx;*/
    /*vec2 k;*/
    /*vec2 c, ht_c;*/
    /*for (int m = -N/2; m < N/2; ++m) {*/
        /*kz = 2.0 * PI * m / L;*/
        /*for (int n = -N/2; n < N/2; ++n) {*/
            /*kx = 2.0 * PI * n / L;*/
            /*k = vec2(kx, kz);*/

            /*kDx = dot(k, x);*/

            /*c = vec2(cos(kDx), sin(kDx));*/
            /*ht_c = cmul(ht(t, n, m), c);*/

            /*height = height + ht_c;*/
            /*normal = normal + vec3(-kx * ht_c.y, 0.0, -kz * ht_c.y);*/
        /*}*/
    /*}*/
    /*height_norm hn;*/
    /*hn.height = height;*/
    /*hn.normal = normal;*/
    /*return hn;*/
/*}*/

// Noise function does not produce the needed behavior in order to produce realistic looking waves
// As it currently stands, a gaussian distribution would have to be implemented from scratch in glsl
// In order to achieve the desired functionality

void main() {       
    vec4 pos = gl_Vertex;
    /*pos.y = pos.y + 0.33*sin(pos.x + time * 0.85) + 0.45*cos(pos.z + time * 0.65);*/
    /*normal = vec3(pos.x, -1.0, pos.z);*/
    /*normal.x = -0.33*cos(pos.x + time * 0.85);*/
    /*normal.y = 1.0;*/
    /*normal.z = 0.45*sin(pos.z + time * 0.65);*/
    /*pos.y = pos.y + snoise(vec2(time, time+17.4));*/
    /*normal = vec3(0.0, 1.0, 0.0);*/
    /*height_norm hn = heightAndNormal(pos.xz, time);*/
    /*pos.y = pos.y + hn.height.x;*/
    /*normal = hn.normal;*/
    halfVector = gl_LightSource[0].halfVector.xyz;
    // Compute the base diffuse color (not the intensity)
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

    // Compute the ambient color
    ambient = gl_FrontMaterial.ambient *
              (gl_LightSource[0].ambient + gl_LightModel.ambient);
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
