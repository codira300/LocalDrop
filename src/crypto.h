#pragma once
#include <sodium.h>
#include <string>

class Crypto
{
public:
    Crypto();

    std::string getPublicKey() const;

    bool deriveClientSessionKeys(const std::string& serverPublicKey);

    bool deriveServerSessionKeys(const std::string& clientPublicKey);

    const unsigned char* getReceiveKey() const;
    const unsigned char* getTransmitKey() const;

private:
    unsigned char m_public_key[crypto_kx_PUBLICKEYBYTES];
    unsigned char m_secret_key[crypto_kx_SECRETKEYBYTES];
    unsigned char m_receive_key[crypto_kx_SESSIONKEYBYTES];
    unsigned char m_transmit_key[crypto_kx_SESSIONKEYBYTES];
};