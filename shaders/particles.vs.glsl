#version 410

uniform float timeElapsedSinceStart;
uniform mat4 modelViewProjection;

in vec3 position;
in vec2 textureCoordinate;


// Generate a randon number between 0 and 1
// Pass instance ID and 0
float random(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float randomBetween0and1(int instanceId) {
    return random(vec2(instanceId, 0));
}

float randomInRange(int instanceId, float minimum, float maximum) {
    return randomBetween0and1(instanceId) * (maximum - minimum) + minimum;
}


void main() {
    // tells us which particle we're dealing with - we're using it to seed the RNG
    // consistently for each particle
    gl_InstanceID;

    vec3 initialPosition = vec3(gl_InstanceID*0.1, 0, 0);
    vec3 initialVelocity = vec3((gl_InstanceID + 1) % 5, 0, 0);
    vec3 acceleration = vec3(0, -5, 0);

    // s = ut + 0.5*a * t^2 for stateless position computation
    float particleAge = mod(timeElapsedSinceStart, 5000.0);

    vec3 particlePosition = initialPosition +
                            initialVelocity*particleAge +
                            0.5 * acceleration * particleAge * particleAge;

    gl_Position = modelViewProjection * vec4(particlePosition, 1.0);
}
