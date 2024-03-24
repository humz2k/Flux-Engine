#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 camPos;
uniform float ka;

out vec4 finalColor;

#define FLUX_MAX_LIGHTS 4

struct Light {
    int enabled;
    int type;
    vec3 cL;
    float kd;
    float ks;
    vec3 pos;
    vec3 L;
    float p;
    float intensity;
};

uniform Light lights[FLUX_MAX_LIGHTS];

vec3 diffuse(vec3 cM, vec3 cL, vec3 N, vec3 L){
    return cM*cL*max(0,dot(N,L));
}

vec3 specular(vec3 cL, vec3 N, vec3 H, float p){
    return cL*pow(max(0,dot(N,H)),p);
}

vec3 calculate_light(Light light, vec3 cM, vec3 N, vec3 V){
    vec3 H = normalize(V + light.L);
    vec3 cL = light.cL;
    vec3 L = light.L;
    float kd = light.kd;
    float ks = light.ks;
    float p = light.p;
    return kd * diffuse(cM,cL,N,L) + ks * specular(cL,N,H,p);
}

void main()
{
    vec4 base_color = texture(texture0, fragTexCoord) * colDiffuse;
    vec3 cM = base_color.xyz;
    vec3 N = fragNormal;
    vec3 V = normalize(camPos - fragPosition);

    vec3 out_col = cM * ka;

    for (int i = 0; i < FLUX_MAX_LIGHTS; i++){
        if (lights[i].enabled == 1){
            out_col += calculate_light(lights[i],cM,N,V);
        }
    }

    finalColor = vec4(out_col.x,out_col.y,out_col.z,base_color.w);

    // gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
}