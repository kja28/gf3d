#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 ambient;
layout(location = 4) in vec3 fragPosWorld;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDirection = normalize(vec3(1.0, -1.0, -1.0)); // Example directional light
    vec3 norm = normalize(fragNormal);
    vec3 viewDirection = normalize(-fragPosWorld); // Assuming camera is at the origin

    float shininess = 512.0;
    vec3 specularColor = vec3(1.0, 1.0, 1.0);

    // Diffuse component
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuseLight = diff * vec3(1.0, 1.0, 1.0); // Assuming white light

    // Specular component
    vec3 halfVector = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
    vec3 specularLight = spec * specularColor;

    // Combine all lighting with texture color
    vec4 baseColor = texture(texSampler, fragTexCoord);
    vec3 color = ambient.xyz * ambient.w + diffuseLight + specularLight;
    outColor = vec4(color, 1.0) * baseColor;
}