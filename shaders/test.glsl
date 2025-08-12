#version 450
layout(local_size_x = 1) in;

// Binding 0: storage buffer
layout(set = 0, binding = 0) buffer InputOutputBuffer {
    int data[];
} bufferData;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    bufferData.data[idx] *= 5;
}
