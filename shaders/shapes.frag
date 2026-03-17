#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

uniform Material material;

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform float alpha;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    vec3 diffuseTexColor = objectColor;
    vec3 specularTexColor = vec3(1.0, 1.0, 1.0);

    // 关键修改点：双面法线处理
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);

    // 法线方向智能处理
        float ndotl = dot(norm, lightDir);
        if (ndotl < 0.0) {
            norm = -norm; // 仅当光照反向时翻转法线
        }

    // 原有光照计算保持不变
    vec3 ambient = diffuseTexColor * light.ambient;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * diffuseTexColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * specularTexColor;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, alpha);
}
