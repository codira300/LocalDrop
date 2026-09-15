#include <iostream>
#include <sodium.h>

int main()
{
    if (sodium_init() < 0){
        std::cout << "libsodium initialization failed\n";
        return 1;
    }

    unsigned char public_key[crypto_kx_PUBLICKEYBYTES];
    unsigned char secret_key[crypto_kx_SECRETKEYBYTES];

    crypto_kx_keypair(public_key, secret_key);

    std::cout << "Key pair generated successfully!\n";

    return 0;
}