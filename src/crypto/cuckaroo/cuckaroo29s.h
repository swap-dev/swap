#ifndef cuckaroo29s_H
#define cuckaroo29s_H

typedef struct siphash_keys__
{
	uint64_t k0;
	uint64_t k1;
	uint64_t k2;
	uint64_t k3;
} siphash_keys;

class Cuckaroo29S
{
	private:

	uint64_t v0;
	uint64_t v1;
	uint64_t v2;
	uint64_t v3;

	public:

	void setsipkeys(const unsigned char*,siphash_keys*);
	void setheader(const unsigned char*, const uint32_t, siphash_keys*);
	uint64_t rotl(uint64_t, uint64_t);
	void sip_round(void);
	void hash24(const uint64_t);
	uint64_t xor_lanes(void);
	uint64_t sipblock(siphash_keys*, const uint32_t,uint64_t*);
	int verify(uint32_t *, siphash_keys*);
	Cuckaroo29S();

	int hash(const void* in, size_t len, uint32_t nonce, uint32_t *edges, void* out);
};
#endif 



