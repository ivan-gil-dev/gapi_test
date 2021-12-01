 #version 440 core

    #define MAX_POINTLIGHT_COUNT 32
  
    layout(location = 0) in vec3 f_Color; 
    layout(location = 1) in vec2 f_UV;
    layout(location = 2) in vec3 f_Normal;
    layout(location = 3) in vec3 f_FragPos;


    struct PointLightDataStruct{
        vec3 pos;
        vec3 color;
        vec4 phongParams;
    };

    layout(set = 0, binding = 1) uniform PointLightData { 
        PointLightDataStruct data;
    }pointLights[MAX_POINTLIGHT_COUNT]; 

    layout(set = 1, binding = 2) uniform sampler2D dTexture;
    layout(set = 1, binding = 3) uniform sampler2D sTexture;

    layout(set = 0, binding = 4) uniform CamPosStruct {vec3 cameraPos;} camPosStruct;

    layout(location = 0) out vec4 fragColor;

    vec3 CalcPointLight(vec3 viewDir, PointLightDataStruct pointLightData){

        vec4 albedoColor = texture( dTexture, f_UV );
        vec3 specularColor = vec3(texture( sTexture, f_UV ));

        if(albedoColor.a<0.5){
            discard;
        }

        vec3 outputValue;

        vec3 lightDir = normalize(pointLightData.pos - f_FragPos);
        vec3 Normal = normalize(f_Normal);

        vec3 reflectDir = reflect(-lightDir,Normal);


        float distance = length(pointLightData.pos - f_FragPos);
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
        
        float attenuation = 1 /(constant + distance * linear + distance * distance * quadratic);
        
        vec3 ambientComponent = pointLightData.phongParams.x * pointLightData.color * vec3(albedoColor);
        vec3 diffuseComponent = pointLightData.phongParams.y * max( dot(Normal, lightDir), 0.0f) * pointLightData.color * vec3(albedoColor);                        
        vec3 specularComponent = pointLightData.phongParams.z * pow(max(dot( viewDir, reflectDir ), 0.0f),32) * pointLightData.color * specularColor;


        ambientComponent *= attenuation;
        diffuseComponent *= attenuation;
        specularComponent *= attenuation;

        outputValue = (ambientComponent + diffuseComponent + specularComponent);

        return outputValue;
    }

    void main() 
    { 
        vec3 result = vec3(0,0,0);
        vec3 viewDir = normalize(camPosStruct.cameraPos - f_FragPos);
        
        for(int i = 0; i < MAX_POINTLIGHT_COUNT; i++){
            if(pointLights[i].data.phongParams.w != 0)
            result += CalcPointLight(viewDir, pointLights[i].data);
        }


        result *= f_Color;

        fragColor = vec4( result, 1.0f ); 
    }   