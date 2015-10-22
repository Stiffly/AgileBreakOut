/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#version 440

uniform vec2 dir;
uniform float res;
uniform float radius;

layout (binding=0) uniform sampler2D WaterTexture;

in VertexData
{
	vec3 Position;
	vec2 TextureCoord;
} Input;

out vec4 frag_Diffuse;

void main()
{
	vec4 WaterTexel = texture(WaterTexture, Input.TextureCoord);

	vec4 sum = vec4(0.0);
	vec2 tc = Input.TextureCoord;

	//How much blur, 1 = blur by one pixel.
	float blur = radius/res;

	float hstep = dir.x;
	float vstep = dir.y;

	float Threshhold = 0.1;

	//apply filter using a 9 tap filter with predefined gaussian weights

	sum += texture(WaterTexture, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
	sum += texture(WaterTexture, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
    sum += texture(WaterTexture, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
    sum += texture(WaterTexture, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;

    sum += texture(WaterTexture, vec2(tc.x, tc.y)) * 0.2270270270;

    sum += texture(WaterTexture, vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
    sum += texture(WaterTexture, vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
    sum += texture(WaterTexture, vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
    sum += texture(WaterTexture, vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;

    float avgColor = sum.r + sum.g + sum.b;
    avgColor = avgColor/3;
    frag_Diffuse = vec4(sum.r, sum.g, sum.b, 1.0);
    if ( avgColor*dir.y > Threshhold ){
    		frag_Diffuse = vec4(0.0, 0.3, 1.0, 1.0);
    }
}