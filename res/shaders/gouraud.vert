uniform int maxLights;
uniform int lightsOn[8];

varying vec4 color;

float d, attenuation;
vec3 position, N, O, L, R;
vec4 Iamb, Idiff, Ispec;

void main(void) {
    // calculate vertex position
    position = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    // calculate N (normal)
    N = normalize(gl_NormalMatrix * gl_Normal);

    // set initial color to ambient color of scene
    color = gl_FrontLightModelProduct.sceneColor;

    // calculate O (points towards observer)
    O = normalize(-position);

    // for light in lights
    for (int i = 0; i < maxLights; i++) {
        // cancel if light is not on
        if (!lightsOn[i]) {
            continue;
        }

        // calculate L (points towards light source)
        if (gl_LightSource[i].position.w == 0) {
            L = normalize(gl_LightSource[i].position.xyz);
        } else {
            L = normalize(gl_LightSource[i].position.xyz - position);
        }

        // cancel if fragment is outside spotlight cutoff
        if (acos(dot(-L, normalize(gl_LightSource[i].spotDirection))) >= radians(gl_LightSource[i].spotCutoff)) {
            continue;
        }

        // calculate R (points in the direction of the reflection)
        R = normalize(reflect(-L, N));

        // calculate d (distance to light)
        d = distance(gl_LightSource[i].position, position);
        // calculate light attenuation
        attenuation = clamp(1.0 / (gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * d + gl_LightSource[i].quadraticAttenuation * d * d), 0.0, 1.0);

        // calculate ambient color
        Iamb = gl_FrontLightProduct[i].ambient;
        // calculate diffuse color
        Idiff = clamp(gl_FrontLightProduct[i].diffuse * max(dot(N, L), 0.0), 0.0, 1.0);
        // calculate specular color
        Ispec = clamp(gl_FrontLightProduct[i].specular * pow(max(dot(R, O), 0.0), gl_FrontMaterial.shininess), 0.0, 1.0);

        // add to final color all color components multiplied by attenuation
        color += attenuation * (Iamb + Idiff + Ispec);
    }

    // set vertex position
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
