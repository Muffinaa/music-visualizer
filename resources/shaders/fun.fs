#version 330 core

in vec2 fragTexCoord;
uniform sampler2D texture0;

out vec4 FragColor;

void main()
{
    vec2 uv = fragTexCoord;
    vec2 texel = 1.0 / textureSize(texture0, 0);

    vec3 base = texture(texture0, uv).rgb;

    float mask = base.g > 0.8 ? 1.0 : 0.0;

    float weights[5] = float[](0.227027, 0.194594, 0.121621, 0.054054, 0.016216);

    float glowAccum = 0.0;

    glowAccum += mask * weights[0];

    for (int i = 1; i < 5; i++)
    {
        float w = weights[i];
        vec2 offset = texel * float(i) * 2.0;

        float m1 = texture(texture0, uv + vec2(offset.x, 0)).g > 0.8 ? 1.0 : 0.0;
        float m2 = texture(texture0, uv - vec2(offset.x, 0)).g > 0.8 ? 1.0 : 0.0;

        float m3 = texture(texture0, uv + vec2(0, offset.y)).g > 0.8 ? 1.0 : 0.0;
        float m4 = texture(texture0, uv - vec2(0, offset.y)).g > 0.8 ? 1.0 : 0.0;

        // optional diagonals (nicer blur)
        float m5 = texture(texture0, uv + offset).g > 0.8 ? 1.0 : 0.0;
        float m6 = texture(texture0, uv - offset).g > 0.8 ? 1.0 : 0.0;

        glowAccum += (m1 + m2 + m3 + m4) * w;
        glowAccum += (m5 + m6) * w * 0.7;
    }

    glowAccum = pow(glowAccum, 1.3);
    vec3 stylized = vec3(1.0, uv.y, uv.x);
    vec3 glowColor = vec3(0.2, 0.7, 1.0);
    vec3 glow = glowColor * glowAccum * 2.5;
    vec3 finalColor = base;
    if (mask > 0.5)
        finalColor = stylized;

    finalColor += glow;
    FragColor = vec4(finalColor, 1.0);
}
