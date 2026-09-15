#include "crypto.h"
#include "error.h"

Crypto::Crypto() {
    if (sodium_init() < 0) ErrorLog("libsodium initialization failed");
 
    if (crypto_kx_keypair(m_public_key, m_secret_key) != 0) ErrorLog("Failed to generate key pair");
}

std::string Crypto::getPublicKey() const
{
    std::string key(
        crypto_kx_PUBLICKEYBYTES * 2 + 1,
        '\0'
    );

    sodium_bin2hex(
        key.data(),
        key.size(),
        m_public_key,
        crypto_kx_PUBLICKEYBYTES
    );

    key.resize(crypto_kx_PUBLICKEYBYTES * 2);

    return key;
}

bool Crypto::deriveClientSessionKeys(
    const std::string& serverPublicKey
)
{
    if (serverPublicKey.size() != crypto_kx_PUBLICKEYBYTES * 2)
    {
        return false;
    }

    unsigned char server_key[crypto_kx_PUBLICKEYBYTES];

    if (sodium_hex2bin(
            server_key,
            sizeof(server_key),
            serverPublicKey.c_str(),
            serverPublicKey.size(),
            nullptr,
            nullptr,
            nullptr
        ) != 0)
    {
        return false;
    }

    return crypto_kx_client_session_keys(
               m_receive_key,
               m_transmit_key,
               m_public_key,
               m_secret_key,
               server_key
           ) == 0;
}

bool Crypto::deriveServerSessionKeys(
    const std::string& clientPublicKey
)
{
    if (clientPublicKey.size() != crypto_kx_PUBLICKEYBYTES * 2)
    {
        return false;
    }

    unsigned char client_key[crypto_kx_PUBLICKEYBYTES];

    if (sodium_hex2bin(
            client_key,
            sizeof(client_key),
            clientPublicKey.c_str(),
            clientPublicKey.size(),
            nullptr,
            nullptr,
            nullptr
        ) != 0)
    {
        return false;
    }

    return crypto_kx_server_session_keys(
               m_receive_key,
               m_transmit_key,
               m_public_key,
               m_secret_key,
               client_key
           ) == 0;
}

const unsigned char* Crypto::getReceiveKey() const
{
    return m_receive_key;
}

const unsigned char* Crypto::getTransmitKey() const
{
    return m_transmit_key;
}