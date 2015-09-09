$output v_Position, v_TextureCoord

SAMPLER2D(DiffuseTexture, 0);

void main() {
    gl_FragColor = texture2D(DiffuseTexture, v_TextureCoord);
}
