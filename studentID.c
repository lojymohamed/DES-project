#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Initial Permutation 
static const int IP[64] = {
    58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,
    62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
    57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,
    61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7
};

// Final Permutation 
static const int FP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

// Expansion permutation: 32 bits to 48 bits 
static const int E[48] = {
    32,1,2,3,4,5,4,5,6,7,8,9,8,9,10,11,12,13,
    12,13,14,15,16,17,16,17,18,19,20,21,20,21,22,23,
    24,25,24,25,26,27,28,29,28,29,30,31,32,1
};

// P permutation 
static const int P[32] = {
    16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,
    2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25
};

// Permuted Choice 1 
static const int PC1[56] = {
    57,49,41,33,25,17,9,1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,19,11,3,60,52,44,36,
    63,55,47,39,31,23,15,7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,21,13,5,28,20,12,4
};

// Permuted Choice 2 
static const int PC2[48] = {
    14,17,11,24,1,5,3,28,15,6,21,10,23,19,12,4,
    26,8,16,7,27,20,13,2,41,52,31,37,47,55,30,40,
    51,45,33,48,44,49,39,56,34,53,46,42,50,36,29,32
};

// Number of left shifts for each round 
static const int SHIFTS[16] = {
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

// DES S-boxes 
static const int SBOX[8][4][16] = {
    {
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    {
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    {
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    {
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    {
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    {
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    {
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    {
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};

 static uint64_t permute(uint64_t input, const int *table,
                        int output_bits, int input_bits) {
    uint64_t output = 0;

    for (int i = 0; i < output_bits; i++) {
        output = (output << 1) |    // Shift output left by 1 and add the next bit from input
                 ((input >> (input_bits - table[i])) & 1ULL); //shift input right by (input_bits - table[i]) and get the least significant bit
    }

    return output;
}

// Generate the 16 round keys 
static void generate_keys(uint64_t key, uint64_t subkeys[16]) {
    uint64_t k = permute(key, PC1, 56, 64);
    //split into two 28-bit halves
    uint32_t C = (uint32_t)((k >> 28) & 0x0FFFFFFF);
    uint32_t D = (uint32_t)(k & 0x0FFFFFFF);

    for (int i = 0; i < 16; i++) {
        int s = SHIFTS[i];

        C = ((C << s) | (C >> (28 - s))) & 0x0FFFFFFF; // Perform left circular shift on C
        D = ((D << s) | (D >> (28 - s))) & 0x0FFFFFFF;

        uint64_t combined = ((uint64_t)C << 28) | D;
        subkeys[i] = permute(combined, PC2, 48, 56);
    }
}

// DES round function 
static uint32_t feistel(uint32_t R, uint64_t subkey) { // operates on 32-bit R and 48-bit subkey
    uint64_t expanded = permute(R, E, 48, 32) ^ subkey;
    uint32_t result = 0;

    for (int i = 0; i < 8; i++) {
        int six = (int)((expanded >> (42 - 6 * i)) & 0x3F); // Extract 6 bits for the current S-box
        int row = ((six & 0x20) >> 4) | (six & 1); // Calculate row index shift the first bit to the second position and OR with the last bit
        int col = (six >> 1) & 0x0F; // Calculate column index (middle 4 bits)

        result = (result << 4) | (uint32_t)SBOX[i][row][col];
    }

    return (uint32_t)permute(result, P, 32, 32);
}

// Encrypt or decrypt one 64-bit block 
static uint64_t des_block(uint64_t block,
                          const uint64_t subkeys[16],
                          int decrypt) {
    uint64_t x = permute(block, IP, 64, 64);

    uint32_t L = (uint32_t)(x >> 32);
    uint32_t R = (uint32_t)x;

    for (int i = 0; i < 16; i++) {
        uint64_t key = subkeys[decrypt ? 15 - i : i];

        uint32_t nextL = R;
        uint32_t nextR = L ^ feistel(R, key);

        L = nextL;
        R = nextR;
    }

    // Swap halves before the final permutation 
    uint64_t combined = ((uint64_t)R << 32) | L;
    return permute(combined, FP, 64, 64);
}

/* Convert 8 bytes to a 64-bit integer */
static uint64_t bytes_to_block(const unsigned char b[8]) {
    uint64_t x = 0;

    for (int i = 0; i < 8; i++)
        x = (x << 8) | b[i];

    return x;
}

/* Convert a 64-bit integer to 8 bytes */
static void block_to_bytes(uint64_t x, unsigned char b[8]) {
    for (int i = 7; i >= 0; i--) {
        b[i] = (unsigned char)(x & 0xFF);
        x >>= 8;
    }
}

int main(int argc, char **argv) {
    if (argc != 5 ||
        (strcmp(argv[1], "e") != 0 &&
         strcmp(argv[1], "d") != 0)) {
        fprintf(stderr,
                "Usage:\n"
                "%s e keyfile plaintext ciphertext\n"
                "%s d keyfile ciphertext plaintext\n",
                argv[0], argv[0]);
        return 1;
    }

    int decrypt = (strcmp(argv[1], "d") == 0);

    /* Read the binary key */
    FILE *kf = fopen(argv[2], "rb"); // Open the key file in binary mode
    if (!kf) {
        perror("Cannot open key file");
        return 1;
    }

    unsigned char key_bytes[8];

    if (fread(key_bytes, 1, 8, kf) != 8) {
        fprintf(stderr, "Key file must contain at least 8 bytes\n");
        fclose(kf);
        return 1;
    }

    fclose(kf);

    uint64_t key = bytes_to_block(key_bytes);
    uint64_t subkeys[16];
    generate_keys(key, subkeys);

    /* Open input and output files */
    FILE *in = fopen(argv[3], "rb");
    if (!in) {
        perror("Cannot open input file");
        return 1;
    }

    FILE *out = fopen(argv[4], "wb");
    if (!out) {
        perror("Cannot open output file");
        fclose(in);
        return 1;
    }

    unsigned char buffer[8];
    int status = 0;

    while (1) {
        size_t n = fread(buffer, 1, 8, in);

        if (n == 0) {
            if (ferror(in)) {
                perror("Read error");
                status = 1;
            }
            break;
        }

        if (n != 8) {
            fprintf(stderr, "Input size must be divisible by 8 bytes\n");
            status = 1;
            break;
        }

        uint64_t block = bytes_to_block(buffer);
        uint64_t result = des_block(block, subkeys, decrypt);
        block_to_bytes(result, buffer);

        if (fwrite(buffer, 1, 8, out) != 8) {
            perror("Write error");
            status = 1;
            break;
        }
    }

    if (fclose(in) != 0) {
        perror("Input close error");
        status = 1;
    }

    if (fclose(out) != 0) {
        perror("Output close error");
        status = 1;
    }

    return status;
}