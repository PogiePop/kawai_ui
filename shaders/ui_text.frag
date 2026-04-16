#version 330 core

in vec2 texCoord;

uniform sampler2D text;
uniform vec4 textColor;

out vec4 FragColor;

void main()
{
    float sdf = texture(text, texCoord).r;

    // ⭐ 核心：边缘重建
    float dist = sdf - 0.5;



    float width = fwidth(sdf); // 抗锯齿关键

    float alpha = smoothstep(-width, width, dist);

    FragColor = vec4(textColor.rgb, textColor.a * alpha);
}