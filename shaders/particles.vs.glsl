#version 410

uniform float timeElapsedSinceStart;
uniform mat4 modelViewProjection;

in vec3 position;
in vec2 textureCoordinate;
out vec2 textureCoordinateFromVertexShader;


// Generate a randon number between 0 and 1
// Pass instance ID and 0
float random(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float randomBetween0and1(float seed) {
    return random(vec2(seed, 0));
}

float randomInRange(float seed, float minimum, float maximum) {
    return randomBetween0and1(seed) * (maximum - minimum) + minimum;
}


void main() {
    // tells us which particle we're dealing with - we're using it to seed the RNG
    // consistently for each particle
    // gl_InstanceID;
    textureCoordinateFromVertexShader = textureCoordinate;

    vec3 initialPosition = vec3(
        randomInRange(gl_InstanceID, -0.25, 0.25),
        0,
        randomInRange(gl_InstanceID+2, -0.25, 0.25)
    );
    vec3 initialVelocity = vec3(
        randomInRange(gl_InstanceID     / 100.0, -0.5, 0.5),
        randomInRange((gl_InstanceID+1) / 100.0, 5, 8),
        randomInRange((gl_InstanceID+2) / 100.0, -0.5, 0.5));

    vec3 acceleration = vec3(0, -10, 0);


    float particleMaxAge = randomInRange(gl_InstanceID, 1000, 2000);
    float particleAge = mod(timeElapsedSinceStart, particleMaxAge);
    particleAge /= 1000.0;

    // s = ut + 0.5*a * t^2 for stateless position computation
    vec3 particlePosition = initialPosition +
                            position +
                            initialVelocity*particleAge +
                            0.5 * acceleration * particleAge * particleAge;

    gl_Position = modelViewProjection * vec4(particlePosition, 1.0);
}
