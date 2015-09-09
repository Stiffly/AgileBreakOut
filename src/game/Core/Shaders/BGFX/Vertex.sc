$input Position, TextureCoord
$output v_Position, v_TextureCoord

void main() {
    gl_Position = u_modelViewProj * vec4(Position, 1.0);

    v_Position = Position;
    v_TextureCoord = TextureCoord;
}
