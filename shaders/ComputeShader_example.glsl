#version 440

// OpenGL 4.4
//layout (local_size_variable) in;

// OpenGL 4.3
layout (local_size_x = 32, local_size_y = 32) in;

layout (binding = 0, rgba32f) readonly uniform image2D input_image;
layout (binding = 1, rgba32f) writeonly uniform image2D output_image;

uniform int radius;

void main()
{
	ivec2 index = ivec2(gl_GlobalInvocationID.xy);
	ivec2 texSize = imageSize(input_image);
	
	if (index.x >= texSize.x || index.y >= texSize.y) return;
	
	vec4 color = imageLoad(input_image, index);
	
	vec4 result_color = vec4(color.x, color.y, color.z, color.w);
	
	imageStore(output_image, index, result_color);
}
