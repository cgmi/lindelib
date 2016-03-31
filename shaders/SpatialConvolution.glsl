//Author: Thomas Lindemeier
//Date: 28 Dez 2015

#version 440

// OpenGL 4.4
//layout (local_size_variable) in;

// OpenGL 4.3
layout (local_size_x = 1, local_size_y = 1) in;

layout (binding = 0, r32f) readonly uniform image2D source;
layout (binding = 1, r32f) writeonly uniform image2D result;
layout (binding = 2, r32f) readonly uniform image2D kernel;
layout (binding = 3, r8ui) readonly uniform uimage2D mask;

void main()
{
    ivec2 index = ivec2(gl_GlobalInvocationID.xy);

    ivec2 texSize = imageSize(source);
    ivec2 radius = imageSize(kernel) / 2;

    if (index.x >= texSize.x || index.y >= texSize.y || index.x < 0  || index.y < 0) return;

    float sum = 0.0;
    for(int k = -radius.x; k <= radius.x; k++)
    {
        for(int l = -radius.y; l <= radius.y; l++)
        {
            ivec2 tPos = ivec2(k, l);
            ivec2 imagePos  = index + tPos;
            ivec2 kernelPos = tPos + radius;

            // out of bounds
            if (imagePos.x >= texSize.x || imagePos.y >= texSize.y
                    || imagePos.x < 0  || imagePos.y < 0) continue;

            // masked out
            if (imageLoad(mask, imagePos).r == 0) continue;

            float w = imageLoad(kernel, kernelPos).r;
            float v = imageLoad(source, imagePos).r;

            sum += w*v;
        }
    }
    imageStore(result, index, vec4(1, 0, 0, 1));
}
