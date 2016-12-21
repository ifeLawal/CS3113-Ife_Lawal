
uniform sampler2D diffuse;

uniform vec2 lightPosition;

varying vec2 texCoordVar;
//varying vec2 varPosition;

/*
float attenuate(float dist, float a, float b) {
    return 1.0 / (1.0 + a*dist + b*dist*dist);
}
*/
void main() {
    /*
    float brightness = attenuate(distance(lightPosition, varPosition), 4.0, 0.0);
    vec4 textureColor = texture2D(diffuse, texCoordVar);
    gl_FragColor = textureColor * brightness;
    gl_FragColor.a = textureColor.a;
     */
    gl_FragColor = texture2D(diffuse, texCoordVar);
}
