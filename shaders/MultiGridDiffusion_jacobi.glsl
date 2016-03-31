//Author: Thomas Lindemeier
//Date: 07 Dez 2015

#version 440

// OpenGL 4.4
//layout (local_size_variable) in;

// OpenGL 4.3
layout (local_size_x = 32, local_size_y = 32) in;

layout (binding = 0, rgba32f) readonly uniform image2D map0;
layout (binding = 1, rgba32f) writeonly uniform image2D map1;


void main()
{
    ivec2 index = ivec2(gl_GlobalInvocationID.xy);

    ivec2 texSize = imageSize(map1);

    if (index.x >= texSize.x || index.y >= texSize.y || index.x < 0  || index.y < 0) return;

    vec4 m00 = imageLoad(map0, index);

    if (m00.a > 0.00)
    {
        imageStore(map1, index,  m00);
    } else
    {
        vec3 M = vec3(0, 0, 0);
        float n = 0;
        ivec2 lx = ivec2(index.x-1, index.y);
        ivec2 rx = ivec2(index.x+1, index.y);
        ivec2 uy = ivec2(index.x, index.y-1);
        ivec2 dy = ivec2(index.x, index.y+1);
        if (lx.x >= 0)
        {
            M += imageLoad(map0, lx).xyz;
            n++;
        }
        if (rx.x < texSize.x)
        {
            M += imageLoad(map0, rx).xyz;
            n++;
        }
        if (uy.y >= 0)
        {
            M += imageLoad(map0, uy).xyz;
            n++;
        }
        if (dy.y < texSize.y)
        {
            M += imageLoad(map0, dy).xyz;
            n++;
        }
        if (n > 0)
        {
            M = M / n;
        }

        imageStore(map1, index,  vec4(M.rgb, 0.0));
    }


}
