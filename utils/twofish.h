#include <array>
#include <string>
#include <vector>

#define KEY_SIZE 32
#define BLOCK_SIZE 16

class twofish256
{
    using word = uint32_t;
    using byte = uint8_t;

    using k_vector_t = std::array <word, 4>;
    using sbox_t = std::array <word, 1024>;
    using subkeys_t = std::array <word, 40>;

    using session_key_t = std::pair <sbox_t, subkeys_t>;

    static const byte P[2][256];
    static const word MDS[4][256];

    session_key_t session_key;
public:
    twofish256();

    using user_key_t = std::array <byte, 32>;
    void make_session_key(const user_key_t& user_key);

    using block_t = std::array <byte, 16>;
    block_t encrypt(const block_t& p);
    block_t decrypt(const block_t& c);
};

uint8_t* encrypt_string(std::string str, std::string key, size_t& size);
std::string decrypt_string(uint8_t* str, std::string key, size_t size);
std::vector <uint8_t> decrypt_dll(uint8_t* str, std::string key, size_t size);
