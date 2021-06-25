#include "custom_sounds.hpp"

__forceinline void setup_sounds()
{
	CreateDirectory("csgo\\sound", nullptr);
	FILE* file = nullptr;

	file = fopen(crypt_str("csgo\\sound\\metallic.wav"), crypt_str("wb"));
	fwrite(metallic, sizeof(unsigned char), 64700, file); //-V575
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\cod.wav"), crypt_str("wb"));
	fwrite(cod, sizeof(unsigned char), 11752, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\bubble.wav"), crypt_str("wb"));
	fwrite(bubble, sizeof(unsigned char), 41550, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\stapler.wav"), crypt_str("wb"));
	fwrite(stapler, sizeof(unsigned char), 105522, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\bell.wav"), crypt_str("wb"));
	fwrite(bell, sizeof(unsigned char), 42154, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\flick.wav"), crypt_str("wb"));
	fwrite(flick, sizeof(unsigned char), 152168, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill1.wav"), crypt_str("wb"));
	fwrite(kill1, sizeof(unsigned char), 248910, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill2.wav"), crypt_str("wb"));
	fwrite(kill2, sizeof(unsigned char), 235086, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill3.wav"), crypt_str("wb"));
	fwrite(kill3, sizeof(unsigned char), 262734, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill4.wav"), crypt_str("wb"));
	fwrite(kill4, sizeof(unsigned char), 253518, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill5.wav"), crypt_str("wb"));
	fwrite(kill5, sizeof(unsigned char), 322652, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill6.wav"), crypt_str("wb"));
	fwrite(kill6, sizeof(unsigned char), 271950, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill7.wav"), crypt_str("wb"));
	fwrite(kill7, sizeof(unsigned char), 318044, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill8.wav"), crypt_str("wb"));
	fwrite(kill8, sizeof(unsigned char), 253518, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill9.wav"), crypt_str("wb"));
	fwrite(kill9, sizeof(unsigned char), 304206, file);
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill10.wav"), crypt_str("wb"));
	fwrite(kill10, sizeof(unsigned char), 327246, file); 
	fclose(file);

	file = fopen(crypt_str("csgo\\sound\\kill11.wav"), crypt_str("wb"));
	fwrite(kill11, sizeof(unsigned char), 396380, file);
	fclose(file);
}