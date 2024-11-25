#version 120

varying vec2 texCoord;
varying vec4 N, V, L;

uniform sampler2D texture;

vec4 ambient, diffuse, specular;

void main()
{
	vec4 tex_color = texture2D(texture, texCoord);
	vec4 NN = normalize(N);
	vec4 LL = normalize(L);
	vec4 VV = normalize(V);
	vec4 H = normalize(L + V);

    specular = pow(max(dot(N, H), 0.0), 50) * vec4(1, 1, 1, 1);
	ambient = 0.4 * tex_color;
	diffuse = max(dot(LL, NN), 0.25) * tex_color;

	gl_FragColor = ambient + diffuse;
}
