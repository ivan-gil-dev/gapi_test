#include "../API/Program.h"
#include "../DataTypes.h"

#ifdef USE_GL

const char* vertexShaderSRC = R"glsl(
    #version 440 core
    layout(location = 0) in vec3 v_Pos;
    layout(location = 1) in vec3 v_Color;
    layout(location = 2) in vec2 v_UV;
    layout(location = 3) in vec3 v_Normal;

    layout(location = 0) out vec3 f_Color;
    layout(location = 1) out vec2 f_UV;
    layout(location = 2) out vec3 f_Normal;
    layout(location = 3) out vec3 f_FragPos;    

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
       

    void main(){

        f_FragPos = vec3( model * vec4( v_Pos, 1.0f ) );
        gl_Position = projection * view * vec4( f_FragPos, 1.0f );
        f_Color = v_Color;
        f_UV = v_UV;
        f_Normal = v_Normal;
    }

)glsl";


const char* fragmentShaderSRC = R"glsl(
    #version 440 core

    #define MAX_POINTLIGHT_COUNT 64
  
    layout(location = 0) in vec3 f_Color; 
    layout(location = 1) in vec2 f_UV;
    layout(location = 2) in vec3 f_Normal;
    layout(location = 3) in vec3 f_FragPos;

    uniform sampler2D dTexture;
    uniform sampler2D sTexture;

    struct PointLightData{
        vec3 pos;
        vec3 color;
        bool isUsed;
        float ambientMultiplier;
        float diffuseMultiplier;
        float specularMultiplier;
    };

    uniform PointLightData pointLights[MAX_POINTLIGHT_COUNT]; 
    uniform vec3 cameraPos;

    out vec4 fragColor; 

    vec3 CalcPointLight(vec3 viewDir, PointLightData pointLight){

        vec4 albedoColor = texture( dTexture, f_UV );
        vec3 specularColor = vec3(texture( sTexture, f_UV ));

        if(albedoColor.a<0.5){
            discard;
        }

        vec3 outputValue;

        vec3 lightDir = normalize(pointLight.pos - f_FragPos);
        vec3 Normal = normalize(f_Normal);

        vec3 reflectDir = reflect(-lightDir,Normal);


        float distance = length(pointLight.pos - f_FragPos);
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
        
        float attenuation = 1 /(constant + distance * linear + distance * distance * quadratic);
        
        vec3 ambientComponent = pointLight.ambientMultiplier * pointLight.color * vec3(albedoColor);
        vec3 diffuseComponent = pointLight.diffuseMultiplier * max( dot(Normal, lightDir), 0.0f) * pointLight.color * vec3(albedoColor);                        
        vec3 specularComponent = pointLight.specularMultiplier * pow(max(dot( viewDir, reflectDir ), 0.0f),32) * pointLight.color * specularColor;


        ambientComponent *= attenuation;
        diffuseComponent *= attenuation;
        specularComponent *= attenuation;

        outputValue = (ambientComponent + diffuseComponent + specularComponent);

        return outputValue;
    }

    void main() 
    { 
        vec3 result = vec3(0,0,0);
        vec3 viewDir = normalize(cameraPos - f_FragPos);
        
        for(int i = 0; i < MAX_POINTLIGHT_COUNT; i++){
            if(pointLights[i].isUsed)
            result += CalcPointLight(viewDir, pointLights[i]);
        }


        result *= f_Color;

        fragColor = vec4( result, 1.0f ); 
    }   
)glsl";


void CheckResult(GLuint shader) { //Проверка результата компиляции шейдера
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);//Получение кода результата компиляции
    if (!result)
    {
        char message[512];
        glGetShaderInfoLog(shader, 512, NULL, message);//Получение деталей ошибки компиляции
        std::cout << "Failed to compile shader" << std::endl;
        std::cout << message << std::endl;
    }
}

Program::Program(ProgramType programType){
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSRC, NULL);
    glCompileShader(vertexShader);
    CheckResult(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSRC, NULL);
    glCompileShader(fragmentShader);
    CheckResult(fragmentShader);

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void Program::UseProgram() {
    glUseProgram(m_program);

}

Program::~Program() {
    glDeleteProgram(m_program);
}

void Program::UniformMatrix4fv(std::string name, glm::mat4& data) {
    GLuint location = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &data[0][0]);
}

void Program::Uniform1i(std::string name, int data) {
    GLuint location = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(location, data);
}

void Program::Uniform3f(std::string name, glm::vec3& data) {
    GLuint location = glGetUniformLocation(m_program, name.c_str());
    glUniform3f(location, data.x, data.y, data.z);
}

void Program::Uniform1f(std::string name, float data) {
    GLuint location = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(location,data);
}


void Program::UniformPointLightData(DataTypes::PointLightData data, int arrayIndex) {
    std::string structLocation = "pointLights[]";
    structLocation.insert(structLocation.size() - 1, std::to_string(arrayIndex));

    std::string posLocation = structLocation + ".pos";
    Uniform3f(posLocation, data.pos);

    std::string colorLocation = structLocation + ".color";
    Uniform3f(colorLocation, data.color);

    std::string isUsedLocation = structLocation + ".isUsed";
    Uniform1i(isUsedLocation, data.isUsed);
        
    std::string ambientMultiplierLocation = structLocation + ".ambientMultiplier";
    Uniform1f(ambientMultiplierLocation, data.ambientMultiplier);

    std::string diffuseMultiplierLocation = structLocation + ".diffuseMultiplier";
    Uniform1f(diffuseMultiplierLocation, data.diffuseMultiplier);

    std::string specularMultiplierLocation = structLocation + ".specularMultiplier";
    Uniform1f(specularMultiplierLocation, data.specularMultiplier);
}

#endif