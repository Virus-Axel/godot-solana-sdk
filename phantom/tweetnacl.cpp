#include "tweetnacl.hpp"

#include <cstdint>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/core/math.hpp>

namespace godot{

const int crypto_secretbox_KEYBYTES = 32;
const int crypto_secretbox_NONCEBYTES = 24;
const int crypto_secretbox_ZEROBYTES = 32;
const int crypto_secretbox_BOXZEROBYTES = 16;
const int crypto_scalarmult_BYTES = 32;
const int crypto_scalarmult_SCALARBYTES = 32;
const int crypto_box_PUBLICKEYBYTES = 32;
const int crypto_box_SECRETKEYBYTES = 32;
const int crypto_box_BEFORENMBYTES = 32;
const int crypto_box_NONCEBYTES = crypto_secretbox_NONCEBYTES;
const int crypto_box_ZEROBYTES = crypto_secretbox_ZEROBYTES;
const int crypto_box_BOXZEROBYTES = crypto_secretbox_BOXZEROBYTES;
const int crypto_sign_BYTES = 64;
const int crypto_sign_PUBLICKEYBYTES = 32;
const int crypto_sign_SECRETKEYBYTES = 64;
const int crypto_sign_SEEDBYTES = 32;
const int crypto_hash_BYTES = 64;

PackedByteArray TweetNacl::_0(){
    PackedByteArray res;
    res.resize(16);
    return res;
}

PackedByteArray TweetNacl::_9(){
    PackedByteArray res;
    res.resize(32);
    res[0] = 9;
    return res;
}

PackedInt64Array TweetNacl::gf0(){
    return gf(PackedInt64Array());
}

PackedInt64Array TweetNacl::gf1(){
    PackedInt64Array param;
    param.resize(1);
    param[0] = 1;
    return gf(param);
}

PackedInt64Array TweetNacl::_121665(){
    PackedInt64Array param;
    param.resize(2);
    param[0] = 0xdb41;
    param[1] = 1;
    return gf(param);
}

PackedInt64Array TweetNacl::D(){
    PackedInt64Array param;
    const uint64_t values[] = {0x78a3, 0x1359, 0x4dca, 0x75eb, 0xd8ab, 0x4141, 0x0a4d, 0x0070, 0xe898, 0x7779, 0x4079, 0x8cc7, 0xfe73, 0x2b6f, 0x6cee, 0x5203};
    param.resize(16);
    for(unsigned int i = 0; i < 16; i++){
        param[i] = values[i];
    }

    return gf(param);
}

PackedInt64Array TweetNacl::D2(){
    PackedInt64Array param;
    const uint64_t values[] = {0xf159, 0x26b2, 0x9b94, 0xebd6, 0xb156, 0x8283, 0x149a, 0x00e0, 0xd130, 0xeef3, 0x80f2, 0x198e, 0xfce7, 0x56df, 0xd9dc, 0x2406};
    param.resize(16);
    for(unsigned int i = 0; i < 16; i++){
        param[i] = values[i];
    }

    return gf(param);
}

PackedInt64Array TweetNacl::X(){
    PackedInt64Array param;
    const uint64_t values[] = {0xd51a, 0x8f25, 0x2d60, 0xc956, 0xa7b2, 0x9525, 0xc760, 0x692c, 0xdc5c, 0xfdd6, 0xe231, 0xc0a4, 0x53fe, 0xcd6e, 0x36d3, 0x2169};
    param.resize(16);
    for(unsigned int i = 0; i < 16; i++){
        param[i] = values[i];
    }

    return gf(param);
}

PackedInt64Array TweetNacl::Y(){
    PackedInt64Array param;
    const uint64_t values[] = {0x6658, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666};
    param.resize(16);
    for(unsigned int i = 0; i < 16; i++){
        param[i] = values[i];
    }

    return gf(param);
}

PackedInt64Array TweetNacl::I(){
    PackedInt64Array param;
    const uint64_t values[] = {0xa0b0, 0x4a0e, 0x1b27, 0xc4ee, 0xe478, 0xad2f, 0x1806, 0x2f43, 0xd7a7, 0x3dfb, 0x0099, 0x2b4d, 0xdf0b, 0x4fc1, 0x2480, 0x2b83};
    param.resize(16);
    for(unsigned int i = 0; i < 16; i++){
        param[i] = values[i];
    }

    return gf(param);
}



PackedByteArray TweetNacl::sigma(){
    const uint8_t values[] = {101, 120, 112, 97, 110, 100, 32, 51, 50, 45, 98, 121, 116, 101, 32, 107};
    PackedByteArray res;
    res.resize(16);
    for(unsigned int i = 0; i < 16; i++){
        res[i] = values[i];
    }

    return res;
}

PackedInt64Array TweetNacl::minusp(){
    const uint32_t values[] = {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252};
    PackedInt64Array res;
    res.resize(17);
    for(unsigned int i = 0; i < 17; i++){
        res[i] = values[i];
    }

    return res;
}

void TweetNacl::_bind_methods(){
    ClassDB::bind_static_method("TweetNacl", D_METHOD("box", "message", "nounce", "public_key", "secret_key"), &TweetNacl::box);
    ClassDB::bind_static_method("TweetNacl", D_METHOD("box_open", "message", "nounce", "public_key", "secret_key"), &TweetNacl::box_open);
    ClassDB::bind_static_method("TweetNacl", D_METHOD("box_keypair"), &TweetNacl::box_keypair);
}

void TweetNacl::pack25519(PackedByteArray &o, const PackedInt64Array &n){
	int64_t b;
	PackedInt64Array m = gf(PackedInt64Array());
	PackedInt64Array t = gf(PackedInt64Array());
	
	for(unsigned int i = 0; i < 16; i++){
		t[i] = n[i];
    }

	car25519(t);
	car25519(t);
	car25519(t);
	
	for(unsigned int j = 0; j < 2; j++){
		m[0] = t[0] - 0xffed;
		for(unsigned int i = 1; i < 15; i++){
			m[i] = t[i] - 0xffff - ((signed_shift_right(m[i-1], 16)) & 1);
			m[i-1] &= 0xffff;
        }

		m[15] = t[15] - 0x7fff - ((signed_shift_right(m[14], 16)) & 1);
		b = (signed_shift_right(m[15], 16)) & 1;
		m[14] &= 0xffff;
		sel25519(t, m, 1-b);
    }
	for(unsigned int i = 0; i < 16; i++){
		o[2*i] = t[i] & 0xff;
		o[2*i+1] = t[i] >> 8;
    }
}


void TweetNacl::car25519(PackedInt64Array &o){
	int64_t c;
	for(unsigned int i = 0; i < 16; i++){
		o[i] += 65536;
		c = floor(o[i] / 65536);
		int64_t un1 = 0;
		int64_t un2 = 0;
		if(i < 15){
			un1 = 1;
        }
		else{
			un1 = 0;
        }
		if(i == 15){
			un2 = 1;
        }
		else{
			un2 = 0;
        }

		o[(i+1)*un1] += c - 1 + 37 * (c-1) * un2;
		o[i] -= (c * 65536);
    }
}

void TweetNacl::inv25519(PackedInt64Array &o, const PackedInt64Array &i){
	PackedInt64Array c = gf(PackedInt64Array());
	for(unsigned int a = 0; a < 16; a++){
		c[a] = i[a];
    }
	for(int a = 253; a > -1; a--){
		S(c, c);
		if(a != 2 && a != 4){
		    M(c, c, i);
        }
    }
	for(unsigned int a = 0; a < 16; a++){
		o[a] = c[a];
    }
}


PackedInt64Array TweetNacl::gf(const PackedInt64Array &list){
	PackedInt64Array ret = PackedInt64Array(list);
	ret.resize(16);
	return ret;
}

void A(PackedInt64Array &o, const PackedInt64Array &a, const PackedInt64Array &b){
	for(unsigned int i = 0; i < 16; i++){
		o[i] = (a[i] + b[i]) | 0;
    }
}

void Z(PackedInt64Array &o, const PackedInt64Array &a, const PackedInt64Array &b){
	for(unsigned int i = 0; i < 16; i++){
		o[i] = (a[i] - b[i]) | 0;
    }
}
		
void TweetNacl::S(PackedInt64Array &o, const PackedInt64Array &a){
	M(o, a, a);
}

void TweetNacl::M(PackedInt64Array &o, const PackedInt64Array &a, const PackedInt64Array &b){
	PackedInt64Array t;
	t.resize(31);
	for(unsigned int i = 0; i < 31; i++){
		t[i] = 0;
    }
	for(unsigned int i = 0; i < 16; i++){
		for(unsigned int j = 0; j < 16; j++){
			t[i + j] += a[i] * b[j];
        }
    }

	for(unsigned int i = 0; i < 15; i++){
		t[i] += 38 * t[i + 16];
    }
	for(unsigned int i = 0; i < 16; i++){
		o[i] = t[i];
    }
	car25519(o);
	car25519(o);
}

void unpack25519(PackedInt64Array &o, const PackedByteArray &n){
	for(unsigned int i = 0; i < 16; i++){
		o[i] = n[2*i] + (n[2*i+1] << 8);
    }
	o[15] = o[15] & 0x7fff;
}

int64_t TweetNacl::signed_shift_right(const int64_t num, const int64_t bits){
	if(num < 0){
		int64_t ret = (abs(num) - 1) >> bits;
		return ~ret;
    }
	else{
		return num >> bits;
    }
}

int64_t TweetNacl::signed_shift_left(const int64_t num, const int32_t bits){
	if(num < 0){
		int64_t ret = (abs(num) - 1) << bits;
		return ~ret;
    }
	else{
		return num << bits;
    }
}

int64_t zero_shift_right(int64_t num, int64_t bits){
	if(num < 0){
		int64_t ret = (((int64_t)1 << 63) + num) >> bits;
		ret += ((int64_t)1 << 62) >> (bits - 1);
		return ret;
    }
	else{
		return num >> bits;
    }
}

void TweetNacl::sel25519(PackedInt64Array &p, PackedInt64Array &q, int64_t b){
	int64_t c = ~(b-1);
	for(unsigned int i = 0; i < 16; i++){
		int64_t t = c & (p[i] ^ q[i]);
		p[i] ^= t;
		q[i] ^= t;
    }
}
	

int64_t ld32(const PackedByteArray &x, int64_t i){
	int64_t u = x[i + 3] & 0xff;
	u = (u << 8) | (x[i + 2] & 0xff);
	u = (u << 8) | (x[i + 1] & 0xff);
	return (u << 8) | (x[i + 0] & 0xff);
}

int64_t TweetNacl::L32(const int64_t x, const int64_t c, const bool is_32){
	if(is_32){
		return signed_shift_left(x, c) % ((uint64_t)1 << 32) | (zero_shift_right(x % ((uint64_t)1 << 32), (32 - c)));
    }
	else{
		return signed_shift_left(x, c) | (zero_shift_right(x, (32 - c)));
    }
}

void TweetNacl::st32(PackedByteArray &x, const int64_t j, int64_t u){
	for(unsigned int i = 0; i < 4; i++){
		x[j+i] = u & 255;
		u = zero_shift_right(u, 8);
    }
}

void add1305(PackedInt64Array &h, const PackedInt64Array &c){
	int64_t u = 0;
	for(unsigned int j = 0; j < 17; j++){
		u = (u + ((h[j] + c[j]) | 0)) | 0;
		h[j] = u & 255;
		u = zero_shift_right(u, 8);
    }
}

bool check_array_types(){
    return true;
}

bool TweetNacl::checkBoxLengths(const PackedByteArray &pk, const PackedByteArray &sk){
	if (pk.size() != crypto_box_PUBLICKEYBYTES){
		return false;
    }
	if (sk.size() != crypto_box_SECRETKEYBYTES){
		return false;
    }
	return true;
}

void TweetNacl::crypto_stream_salsa20_xor(PackedByteArray &c, int64_t cpos, const PackedByteArray &m, int64_t mpos, int64_t b, const PackedByteArray &n, const PackedByteArray &k){
	PackedByteArray x;
    PackedByteArray z;
	z.resize(16);
	x.resize(64);

	if(!b){
		return;
    }
	for(unsigned int i = 0; i < 16; i++){
		z[i] = 0;
    }
	for(unsigned int i = 0; i < 8; i++){
		z[i] = n[i];
    }

	while(b >= 64){
		crypto_core_salsa20(x, z, k, sigma());
		for(unsigned int i = 0; i < 64; i++){
			if(!m.is_empty()){
				c[cpos + i] = m[mpos+i] ^ x[i];
            }
			else{
				c[cpos + i] = 0 ^ x[i];
            }
        }
		int32_t u = 1;
		for(unsigned int i = 8; i < 16; i++){
			u = u + (z[i] & 0xff) | 0;
			z[i] = u & 0xff;
			u = zero_shift_right(u, 8);
        }
		
		b -= 64;
		cpos += 64;
		if(!m.is_empty()){
			mpos += 64;
        }
    }
	if(b > 0){
		crypto_core_salsa20(x ,z ,k , sigma());
		for(unsigned int i = 0; i < b; i++){
			if(!m.is_empty()){
				c[cpos+i] = m[mpos+i] ^ x[i];
            }
			else{
				c[cpos+i] = 0 ^ x[i];
            }
        }
    }
}

void TweetNacl::crypto_onetimeauth(PackedByteArray &out, int64_t outpos, const PackedByteArray &m, int64_t mpos, int64_t n, const PackedByteArray &k){
	int64_t s;
	int64_t u;
	PackedInt64Array x;
    PackedInt64Array r;
    PackedInt64Array h;
    PackedInt64Array c;
    PackedInt64Array g;
	x.resize(17);
	r.resize(17);
	h.resize(17);
	c.resize(17);
	g.resize(17);
	for(unsigned int j = 0; j < 17; j++){
		r[j] = 0;
		h[j] = 0;
    }

	for(unsigned int j = 0; j < 16; j++){
		r[j] = k[j];
    }

	r[3] &= 15;
	r[4] &= 252;
	r[7] &= 15;
	r[8] &= 252;
	r[11] &= 15;
	r[12] &= 252;
	r[15] &= 15;

	while(n > 0){
		for(unsigned int j = 0; j < 17; j++){
			c[j] = 0;
        }
		for(unsigned int j = 0; j < n; j++){
			if(j >= 16){
				break;
            }
			c[j] = m[mpos + j];
        }
		c[Math::min((int64_t)16, n)] = 1;
		mpos += Math::min((int64_t)16, n);
		n -= Math::min((int64_t)16, n);
		add1305(h, c);
		for(unsigned int i = 0; i < 17; i++){
			x[i] = 0;
			for(unsigned int j = 0; j < 17; j++){
				if (j <= i){
					x[i] = (x[i] + (h[j] * (r[i - j])) | 0) | 0;
                }
				else{
					x[i] = (x[i] + (h[j] * ((320 * r[i + 17 - j])|0)) | 0) | 0;
                }
            }
        }
		for(unsigned int i = 0; i < 17; i++){
			h[i] = x[i];
        }
		u = 0;
		for(unsigned int j = 0; j < 16; j++){
			u = (u + h[j]) | 0;
			h[j] = u & 255;
			u = zero_shift_right(u % ((uint64_t)1 << 32), 8);
        }

		u = (u + h[16]) | 0; h[16] = u & 3;
		u = (5 * zero_shift_right(u % ((uint64_t)1 << 32), 2)) | 0;
		for(unsigned int j = 0; j < 16; j++){
			u = (u + h[j]) | 0;
			h[j] = u & 255;
			u = zero_shift_right(u % ((uint64_t)1 << 32), 8);
        }

		u = (u + h[16]) | 0; h[16] = u;
    }

	for(unsigned int j = 0; j < 17; j++){
		g[j] = h[j];
    }

	add1305(h, minusp());
	s = (-zero_shift_right(h[16] % ((uint64_t)1 << 32), 7) | 0);
	for(unsigned int j = 0; j < 17; j++){
		h[j] ^= s & (g[j] ^ h[j]);
    }

	for(unsigned int j = 0; j < 16; j++){
		c[j] = k[j + 16];
    }
	c[16] = 0;
	add1305(h,c);

	for(unsigned int j = 0; j < 16; j++){
		out[outpos+j] = h[j];
    }
}

void TweetNacl::core(PackedByteArray &out, const PackedByteArray inp, const PackedByteArray &k, const PackedByteArray &c, const bool h){
	PackedInt64Array w;
    PackedInt64Array x;
    PackedInt64Array y;
    PackedInt64Array t;

	w.resize(16);
	x.resize(16);
	y.resize(16);
	t.resize(4);

	for(unsigned int i = 0; i < 4; i++){
		x[5 * i] = ld32(c, 4 * i) % ((uint64_t)1 << 32);
		x[1 + i] = ld32(k, 4 * i) % ((uint64_t)1 << 32);
		x[6 + i] = ld32(inp, 4 * i) % ((uint64_t)1 << 32);
		x[11 + i] = ld32(k, 16 + 4 * i) % ((uint64_t)1 << 32);
    }

	for(unsigned int i = 0; i < 16; i++){
		y[i] = x[i];
    }

	for(unsigned int i = 0; i < 20; i++){
		for(unsigned int j = 0; j < 4; j++){
			for(unsigned int m = 0; m < 4; m++){
				t[m] = x[(5 * j + 4 * m) % 16];
            }
			t[1] = t[1] ^ (L32((t[0] + t[3]), 7, true) % ((uint64_t)1 << 32));
			t[2] = t[2] ^ (L32((t[1] + t[0]), 9, true) % ((uint64_t)1 << 32));
			t[3] = t[3] ^ (L32((t[2] + t[1]), 13, true) % ((uint64_t)1 << 32));
			t[0] = t[0] ^ (L32((t[3] + t[2]), 18, true) % ((uint64_t)1 << 32));
			for(unsigned int m = 0; m < 4; m++){
				w[4 * j + (j + m) % 4] = t[m];
            }
        }
		for(unsigned int m = 0; m < 16; m++){
			x[m] = w[m];
        }
    }
	if(h){
		for(unsigned int i = 0; i < 16; i++){
			x[i] = (x[i] + y[i]) | 0;
        }
		for(unsigned int i = 0; i < 4; i++){
			x[5 * i] = (x[5 * i] - ld32(c, 4 * i)) | 0;
			x[6 + i] = (x[6 + i] - ld32(inp, 4 * i)) | 0;
        }
		for(unsigned int i = 0; i < 4; i++){
			st32(out,4*i,x[5*i]);
			st32(out,16+4*i,x[6+i]);
        }
    }
	else{
		for(unsigned int i = 0; i < 16; i++){
			st32(out, 4 * i, (x[i] + y[i]) | 0);
        }
    }
}

void TweetNacl::crypto_core_salsa20(PackedByteArray &out, const PackedByteArray &inp, const PackedByteArray &k, const PackedByteArray &c){
	core(out, inp, k, c, false);
}

void TweetNacl::crypto_core_hsalsa20(PackedByteArray &out, const PackedByteArray &inp, const PackedByteArray &k, const PackedByteArray &c){
	core(out,inp,k,c,true);
}

void TweetNacl::crypto_stream_xor(PackedByteArray &c, const int64_t cpos, const PackedByteArray &m, const int64_t mpos, const int64_t d, const PackedByteArray &n, const PackedByteArray &k){
	PackedByteArray s;
	s.resize(32);
	crypto_core_hsalsa20(s, n, k, sigma());
	crypto_stream_salsa20_xor(c, cpos, m, mpos, d, n.slice(16), s);
}

void TweetNacl::crypto_box_beforenm(PackedByteArray &k, const PackedByteArray &y, const PackedByteArray &x){
	PackedByteArray s;
	s.resize(32);
	crypto_scalarmult(s, x, y);

	return crypto_core_hsalsa20(k, _0(), s, sigma());
}

bool TweetNacl::crypto_secretbox(PackedByteArray &c, const PackedByteArray &m, int64_t d, const PackedByteArray &n, const PackedByteArray &k){
	if (d < 32){
		return false;
    }
	crypto_stream_xor(c, 0, m, 0, d, n, k);
	crypto_onetimeauth(c, 16, c, 32, d - 32, c);
	for(unsigned int i = 0; i < 16; i++){
		c[i] = 0;
    }
	return true;
}

PackedByteArray TweetNacl::box_before(const PackedByteArray &publicKey, const PackedByteArray &secretKey){
	//checkArrayTypes(publicKey, secretKey);
	//checkBoxLengths(publicKey, secretKey);
	PackedByteArray k;
	k.resize(crypto_box_BEFORENMBYTES);
	crypto_box_beforenm(k, publicKey, secretKey);
	return k;
}
	
bool TweetNacl::checkLengths(const PackedByteArray &k, const PackedByteArray &n){
	if (k.size() != crypto_secretbox_KEYBYTES){
		return false;
    }
	if (n.size() != crypto_secretbox_NONCEBYTES){
		return false;
    }

    return true;
}

PackedByteArray TweetNacl::secretbox(const PackedByteArray &msg, const PackedByteArray &nonce, const PackedByteArray &key){
	//checkArrayTypes(msg, nonce, key);
	//checkLengths(key, nonce);
	PackedByteArray m;
    PackedByteArray c;
	m.resize(crypto_secretbox_ZEROBYTES + msg.size());
	c.resize(m.size());
	for(unsigned int i = 0; i < msg.size(); i++){
		m[i + crypto_secretbox_ZEROBYTES] = msg[i];
    }
	crypto_secretbox(c, m, m.size(), nonce, key);
	return c.slice(crypto_secretbox_BOXZEROBYTES);
}

void TweetNacl::crypto_stream_salsa20(PackedByteArray &c, const int64_t cpos, const int64_t d,const PackedByteArray &n, const PackedByteArray &k){
	return crypto_stream_salsa20_xor(c, cpos, PackedByteArray(), 0, d, n, k);
}

void TweetNacl::crypto_stream(PackedByteArray &c, const int64_t cpos, const int64_t d, const PackedByteArray &n, const PackedByteArray &k){
	PackedByteArray s;
	s.resize(32);
	crypto_core_hsalsa20(s, n, k, sigma());
	return crypto_stream_salsa20(c,cpos,d,n.slice(16),s);
}

int64_t vn(const PackedByteArray &x, int64_t xi, const PackedByteArray &y, const int64_t yi, const int64_t n){
	int64_t d = 0;
	for(unsigned int i = 0; i < n; i++){
		d |= x[xi+i]^y[yi+i];
    }
	return (1 & zero_shift_right((d - 1), 8)) - 1;
}

int64_t crypto_verify_16(const PackedByteArray &x, int64_t xi, const PackedByteArray &y, const int64_t yi){
	return vn(x,xi,y,yi,16);
}

int64_t TweetNacl::crypto_onetimeauth_verify(const PackedByteArray &h, const int64_t hpos, const PackedByteArray &m, const int64_t mpos, const int64_t n, const PackedByteArray &k){
	PackedByteArray x;
	x.resize(16);
	crypto_onetimeauth(x, 0, m, mpos, n, k);
	return crypto_verify_16(h, hpos, x, 0);
}

bool TweetNacl::crypto_secretbox_open(PackedByteArray &m, const PackedByteArray &c, int64_t d, const PackedByteArray &n, const PackedByteArray &k){
	PackedByteArray x;
	x.resize(32);
	if (d < 32){
		return false;
    }
	crypto_stream(x, 0, 32, n, k);
	if (crypto_onetimeauth_verify(c, 16, c, 32, d - 32, x) != 0){
		return false;
    }
	crypto_stream_xor(m, 0, c, 0, d, n, k);
	for(unsigned int i = 0; i < 32; i++){
		m[i] = 0;
    }

	return true;
}

PackedByteArray TweetNacl::secretbox_open(const PackedByteArray &box, const PackedByteArray &nonce, const PackedByteArray &key){
	checkLengths(key, nonce);
	PackedByteArray c;
	c.resize(crypto_secretbox_BOXZEROBYTES + box.size());
	PackedByteArray m;
	m.resize(c.size());
	for(unsigned int i = 0; i < box.size(); i++){
		c[i+crypto_secretbox_BOXZEROBYTES] = box[i];
    }
	if(c.size() < 32){
		return PackedByteArray();
    }
	if(!crypto_secretbox_open(m, c, c.size(), nonce, key)){
		return PackedByteArray();
    }
	return m.slice(crypto_secretbox_ZEROBYTES);
}

PackedByteArray TweetNacl::box(const PackedByteArray &msg, const PackedByteArray &nonce, const PackedByteArray &publicKey, const PackedByteArray &secretKey){
	PackedByteArray k = box_before(publicKey, secretKey);
	return secretbox(msg, nonce, k);
}

PackedByteArray TweetNacl::box_open(const PackedByteArray &msg, const PackedByteArray &nonce, const PackedByteArray &publicKey, const PackedByteArray &secretKey){
	PackedByteArray k = box_before(publicKey, secretKey);
	return secretbox_open(msg, nonce, k);
}

void TweetNacl::crypto_scalarmult(PackedByteArray &q, const PackedByteArray &n, const PackedByteArray &p){
    PackedByteArray z;
	z.resize(32);
	PackedInt64Array x;
	x.resize(80);
	uint64_t r;
	PackedInt64Array a = gf0();
	PackedInt64Array b = gf0();
	PackedInt64Array c = gf0();
	PackedInt64Array d = gf0();
	PackedInt64Array e = gf0();
	PackedInt64Array f = gf0();
	for(unsigned int i = 0; i < 31; i++){
		z[i] = n[i];
    }
	z[31] = (n[31] & 127) | 64;
	z[0] &= 248;
	unpack25519(x,p);
	for(unsigned int i = 0; i < 16; i++){
		b[i] = x[i];
		d[i] = 0;
		a[i] = 0;
		c[i] = 0;
    }

	a[0] = 1;
	d[0] = 1;
	for(int i = 254; i > -1; i--){
		r = (zero_shift_right(z[zero_shift_right(i, 3)], (i & 7))) & 1;
		sel25519(a,b,r);
		sel25519(c,d,r);
		A(e,a,c);
		Z(a,a,c);
		A(c,b,d);
		Z(b,b,d);
		S(d,e);
		S(f,a);
		M(a,c,a);
		M(c,b,e);
		A(e,a,c);
		Z(a,a,c);
		S(b,a);
		Z(c,d,f);
		M(a,c,_121665());
		A(a,a,d);
		M(c,c,a);
		M(a,d,f);
		M(d,b,x);
		S(b,e);
		sel25519(a,b,r);
		sel25519(c,d,r);
    }

	for(unsigned int i = 0; i < 16; i++){
		x[i+16] = a[i];
		x[i+32] = c[i];
		x[i+48] = b[i];
		x[i+64] = d[i];
    }

	PackedInt64Array x32 = x.slice(32);
	PackedInt64Array x16 = x.slice(16);
	inv25519(x32, x32);
	M(x16, x16, x32);
	pack25519(q, x16);
}

void randombytes(PackedByteArray &x, unsigned int size){
    PackedByteArray *result = memnew(PackedByteArray);
    
    result->resize(size);
    RandomNumberGenerator rnd;
	for(unsigned int i = 0; i < size; i++){
		x[i] = rnd.randi() % 256;
    }
}

void TweetNacl::crypto_box_keypair(PackedByteArray& y, PackedByteArray &x){
	randombytes(x, 32);
	return crypto_scalarmult_base(y, x);
}

void TweetNacl::crypto_scalarmult_base(PackedByteArray &q, const PackedByteArray &n){
	return crypto_scalarmult(q, n, _9());
}

Array TweetNacl::box_keypair(){
	PackedByteArray *pk = memnew(PackedByteArray);
	pk->resize(crypto_box_PUBLICKEYBYTES);
	PackedByteArray *sk = memnew(PackedByteArray);
	sk->resize(crypto_box_SECRETKEYBYTES);
	crypto_box_keypair(*pk, *sk);

    Array result;
    result.append(*sk);
    result.append(*pk);

	return result;
}

}
