#include "tweetnacl.hpp"

#include <cstdint>
#include <godot_cpp/classes/node.hpp>

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

PackedByteArray _0;
PackedByteArray _9;


PackedInt64Array gf0 = gf(PackedInt64Array());
PackedInt64Array gf1 = gf([1]);
var _121665 = gf([0xdb41, 1])
var D = gf([0x78a3, 0x1359, 0x4dca, 0x75eb, 0xd8ab, 0x4141, 0x0a4d, 0x0070, 0xe898, 0x7779, 0x4079, 0x8cc7, 0xfe73, 0x2b6f, 0x6cee, 0x5203])
var D2 = gf([0xf159, 0x26b2, 0x9b94, 0xebd6, 0xb156, 0x8283, 0x149a, 0x00e0, 0xd130, 0xeef3, 0x80f2, 0x198e, 0xfce7, 0x56df, 0xd9dc, 0x2406])
var X = gf([0xd51a, 0x8f25, 0x2d60, 0xc956, 0xa7b2, 0x9525, 0xc760, 0x692c, 0xdc5c, 0xfdd6, 0xe231, 0xc0a4, 0x53fe, 0xcd6e, 0x36d3, 0x2169])
var Y = gf([0x6658, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666])
var I = gf([0xa0b0, 0x4a0e, 0x1b27, 0xc4ee, 0xe478, 0xad2f, 0x1806, 0x2f43, 0xd7a7, 0x3dfb, 0x0099, 0x2b4d, 0xdf0b, 0x4fc1, 0x2480, 0x2b83])

var sigma = PackedByteArray([101, 120, 112, 97, 110, 100, 32, 51, 50, 45, 98, 121, 116, 101, 32, 107]);

var minusp = PackedInt32Array([
  5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252
]);

func pack25519(o, n):
	var b;
	var m = gf([])
	var t = gf([]);
	
	for i in range(16):
		t[i] = n[i];
	car25519(t);
	car25519(t);
	car25519(t);
	
	for j in range(2):
		m[0] = t[0] - 0xffed;
		for i in range(1, 15):
			m[i] = t[i] - 0xffff - ((signed_shift_right(m[i-1], 16)) & 1);
			m[i-1] &= 0xffff;

		m[15] = t[15] - 0x7fff - ((signed_shift_right(m[14], 16)) & 1);
		b = (signed_shift_right(m[15], 16)) & 1;
		m[14] &= 0xffff;
		sel25519(t, m, 1-b);
	for i in range(16):
		o[2*i] = t[i] & 0xff;
		o[2*i+1] = t[i]>>8;


func car25519(o):
	var c;
	for i in range(16):
		o[i] += 65536;
		c = floor(o[i] / 65536);
		var un1 = 0
		var un2 = 0
		if i < 15:
			un1 = 1
		else:
			un1 = 0
		if i == 15:
			un2 = 1
		else:
			un2 = 0
		o[(i+1)*un1] += c - 1 + 37 * (c-1) * un2;
		o[i] -= (c * 65536);

func inv25519(o, i):
	var c = gf([]);
	for a in range(16):
		c[a] = i[a];
	for a in range(253, -1, -1):
		S(c, c)
		if(a != 2 && a != 4):
			M(c, c, i)
	for a in range(16):
		o[a] = c[a];


func gf(list):
	var ret = PackedInt64Array(list)
	ret.resize(16)
	return ret

func A(o, a, b):
	for i in range(16):
		o[i] = (a[i] + b[i]) | 0

func Z(o, a, b):
	for i in range(16):
		o[i] = (a[i] - b[i]) | 0
		
func S(o, a):
	M(o, a, a);

func M(o, a, b):
	var t = PackedInt64Array()
	t.resize(31)
	for i in range(31):
		t[i] = 0
	for i in range(16):
		for j in range(16):
			t[i + j] += a[i] * b[j]

	for i in range(15):
		t[i] += 38 * t[i + 16];
	for i in range(16):
		o[i] = t[i];
	car25519(o);
	car25519(o);

func unpack25519(o, n):
	for i in range(16):
		o[i] = n[2*i] + (n[2*i+1] << 8)
	o[15] = o[15] & 0x7fff;

func signed_shift_right(num, bits):
	if num < 0:
		var ret = (abs(num) - 1) >> bits
		return ~ret
	else:
		return num >> bits

func signed_shift_left(num, bits):
	if num < 0:
		var ret = (abs(num) - 1) << bits
		return ~ret
	else:
		return num << bits

func zero_shift_right(num, bits):
	if num < 0:
		var ret = ((1 << 63) + num) >> bits
		ret += (1 << 62) >> (bits - 1)
		return ret
	else:
		return num >> bits

func sel25519(p, q, b):
	var t
	var c = ~(b-1);
	for i in range(16):
		t = c & (p[i] ^ q[i]);
		p[i] ^= t;
		q[i] ^= t;
	

func ld32(x, i):
	var u = x[i + 3] & 0xff;
	u = (u << 8) | (x[i + 2] & 0xff);
	u = (u << 8) | (x[i + 1] & 0xff);
	return (u << 8) | (x[i + 0] & 0xff)

func L32(x, c, is_32 = false):
	if is_32:
		return signed_shift_left(x, c) % (1 << 32) | (zero_shift_right(x % (1 << 32), (32 - c)));
	else:
		return signed_shift_left(x, c) | (zero_shift_right(x, (32 - c)));

func st32(x, j, u):
	for i in range(4):
		x[j+i] = u & 255
		u = zero_shift_right(u, 8)

func add1305(h, c):
	var u = 0;
	for j in range(17):
		u = (u + ((h[j] + c[j]) | 0)) | 0;
		h[j] = u & 255;
		u = zero_shift_right(u, 8)

func check_array_types():
	# Use to chect that arguments are PackedByteArray
	pass

func checkBoxLengths(pk, sk):
	if (pk.size() != crypto_box_PUBLICKEYBYTES):
		return false
	if (sk.size() != crypto_box_SECRETKEYBYTES):
		return false
	return true

func crypto_stream_salsa20_xor(c,cpos,m,mpos,b,n,k):
	var z = PackedByteArray()
	z.resize(16)
	var x = PackedByteArray()
	x.resize(64)

	var u
	if !b:
		return 0
	for i in range(16):
		z[i] = 0
	for i in range(8):
		z[i] = n[i]

	while b >= 64:
		crypto_core_salsa20(x,z,k,sigma);
		for i in range(64):
			if m:
				c[cpos + i] = m[mpos+i] ^ x[i]
			else:
				c[cpos + i] = 0 ^ x[i]
		u = 1
		for i in range(8, 16):
			u = u + (z[i] & 0xff) | 0;
			z[i] = u & 0xff;
			u = zero_shift_right(u, 8);
		

		b -= 64;
		cpos += 64;
		if m:
			mpos += 64;
	if (b > 0):
		crypto_core_salsa20(x,z,k,sigma);
		for i in range(b):
			if m:
				c[cpos+i] = m[mpos+i] ^ x[i]
			else:
				c[cpos+i] = 0 ^ x[i]
	return 0;

func crypto_onetimeauth(out, outpos, m, mpos, n, k):
	var s
	var u
	var x = PackedInt64Array()
	x.resize(17)
	var r = PackedInt64Array()
	r.resize(17)
	var h = PackedInt64Array()
	h.resize(17)
	var c = PackedInt64Array()
	c.resize(17)
	var g = PackedInt64Array()
	g.resize(17)
	for j in range(17):
		r[j] = 0
		h[j] = 0
	for j in range(16):
		r[j] = k[j]
	r[3] &= 15;
	r[4] &= 252;
	r[7] &= 15;
	r[8] &= 252;
	r[11] &= 15;
	r[12] &= 252;
	r[15] &= 15;

	while n > 0:
		for j in range(17):
			c[j] = 0
		for j in range(n):
			if j >= 16:
				break
			c[j] = m[mpos + j]
		c[min(16, n)] = 1
		mpos += min(16, n)
		n -= min(16, n)
		add1305(h,c)
		for i in range(17):
			x[i] = 0;
			for j in range(17):
				if (j <= i):
					x[i] = (x[i] + (h[j] * (r[i - j])) | 0) | 0;
				else:
					x[i] = (x[i] + (h[j] * ((320 * r[i + 17 - j])|0)) | 0) | 0;
		for i in range(17):
			h[i] = x[i]
		u = 0;
		for j in range(16):
			u = (u + h[j]) | 0;
			h[j] = u & 255;
			u = zero_shift_right(u % (1 << 32), 8);

		u = (u + h[16]) | 0; h[16] = u & 3;
		u = (5 * zero_shift_right(u % (1 << 32), 2)) | 0;
		for j in range(16):
			u = (u + h[j]) | 0;
			h[j] = u & 255;
			u = zero_shift_right(u % (1 << 32), 8);

		u = (u + h[16]) | 0; h[16] = u;

	for j in range(17):
		g[j] = h[j];
	add1305(h, minusp);
	s = (-zero_shift_right(h[16] % (1 << 32), 7) | 0);
	for j in range(17):
		h[j] ^= s & (g[j] ^ h[j]);

	for j in range(16):
		c[j] = k[j + 16];
	c[16] = 0;
	add1305(h,c);
	for j in range(16):
		out[outpos+j] = h[j];
	return 0;

func core(out, inp, k, c, h):
	var w = PackedInt64Array()
	w.resize(16)
	var x = PackedInt64Array()
	x.resize(16)
	var y = PackedInt64Array()
	y.resize(16)
	var t = PackedInt64Array()
	t.resize(4)

	for i in range(4):
		x[5 * i] = ld32(c, 4 * i) % (1 << 32);
		x[1 + i] = ld32(k, 4 * i) % (1 << 32);
		x[6 + i] = ld32(inp, 4 * i) % (1 << 32);
		x[11 + i] = ld32(k, 16 + 4 * i) % (1 << 32);

	for i in range(16):
		y[i] = x[i]

	for i in range(20):
		for j in range(4):
			for m in range(4):
				t[m] = x[(5 * j + 4 * m) % 16]
			t[1] = t[1] ^ (L32((t[0] + t[3]), 7, true) % (1 << 32))
			t[2] = t[2] ^ (L32((t[1] + t[0]), 9, true) % (1 << 32))
			t[3] = t[3] ^ (L32((t[2] + t[1]), 13, true) % (1 << 32))
			t[0] = t[0] ^ (L32((t[3] + t[2]), 18, true) % (1 << 32))
			for m in range(4):
				w[4 * j + (j + m) % 4] = t[m]
		for m in range(16):
			x[m] = w[m];
	if h:
		for i in range(16):
			x[i] = (x[i] + y[i]) | 0
		for i in range(4):
			x[5 * i] = (x[5 * i] - ld32(c, 4 * i)) | 0;
			x[6 + i] = (x[6 + i] - ld32(inp, 4 * i)) | 0;
		for i in range(4):
			st32(out,4*i,x[5*i]);
			st32(out,16+4*i,x[6+i]);
	else:
		for i in range(16):
			st32(out, 4 * i, (x[i] + y[i]) | 0);

func crypto_core_salsa20(out, inp, k, c):
	core(out, inp, k, c, false)
	return 0

func crypto_core_hsalsa20(out,inp,k,c):
	core(out,inp,k,c,true)
	return 0

func crypto_stream_xor(c,cpos,m,mpos,d,n,k):
	var s = PackedByteArray()
	s.resize(32)
	crypto_core_hsalsa20(s,n,k,sigma);
	return crypto_stream_salsa20_xor(c,cpos,m,mpos,d,n.slice(16),s);

func crypto_box_beforenm(k, y, x):
	var s = PackedByteArray();
	s.resize(32)
	crypto_scalarmult(s, x, y);
	print("s is: ", s)
	return crypto_core_hsalsa20(k, _0, s, sigma);

func crypto_secretbox(c, m, d, n, k):
	if (d < 32):
		return -1;
	crypto_stream_xor(c, 0, m, 0, d, n, k);
	crypto_onetimeauth(c, 16, c, 32, d - 32, c);
	for i in range(16):
		c[i] = 0
	return 0;

func box_before(publicKey, secretKey):
	#checkArrayTypes(publicKey, secretKey);
	checkBoxLengths(publicKey, secretKey);
	var k = PackedByteArray()
	k.resize(crypto_box_BEFORENMBYTES)
	crypto_box_beforenm(k, publicKey, secretKey);
	return k;
	
func checkLengths(k, n):
	if (k.size() != crypto_secretbox_KEYBYTES):
		return false
	if (n.size() != crypto_secretbox_NONCEBYTES):
		return false

func secretbox(msg, nonce, key):
	#checkArrayTypes(msg, nonce, key);
	#
	checkLengths(key, nonce);
	var m = PackedByteArray();
	m.resize(crypto_secretbox_ZEROBYTES + msg.size())
	var c = PackedByteArray();
	c.resize(m.size())
	for i in range(msg.size()):
		m[i + crypto_secretbox_ZEROBYTES] = msg[i];
	crypto_secretbox(c, m, m.size(), nonce, key);
	return c.slice(crypto_secretbox_BOXZEROBYTES);

func crypto_stream_salsa20(c,cpos,d,n,k):
	return crypto_stream_salsa20_xor(c,cpos,null,0,d,n,k);

func crypto_stream(c,cpos,d,n,k):
	var s = PackedByteArray()
	s.resize(32)
	crypto_core_hsalsa20(s,n,k,sigma);
	return crypto_stream_salsa20(c,cpos,d,n.slice(16),s);

func vn(x, xi, y, yi, n):
	var d = 0;
	for i in range(n):
		d |= x[xi+i]^y[yi+i];
	return (1 & zero_shift_right((d - 1), 8)) - 1;

func crypto_verify_16(x, xi, y, yi):
	return vn(x,xi,y,yi,16);

func crypto_onetimeauth_verify(h, hpos, m, mpos, n, k):
	var x = PackedByteArray()
	x.resize(16)
	crypto_onetimeauth(x,0,m,mpos,n,k);
	return crypto_verify_16(h,hpos,x,0);

func crypto_secretbox_open(m,c,d,n,k):
	var x = PackedByteArray()
	x.resize(32)
	if (d < 32):
		return -1;
	crypto_stream(x,0,32,n,k);
	if (crypto_onetimeauth_verify(c, 16,c, 32,d - 32,x) != 0):
		return -1;
	crypto_stream_xor(m,0,c,0,d,n,k);
	for i in range(32):
		m[i] = 0;
	return 0;

func secretbox_open(box, nonce, key):
	checkLengths(key, nonce);
	var c = PackedByteArray()
	c.resize(crypto_secretbox_BOXZEROBYTES + box.size())
	var m = PackedByteArray()
	m.resize(c.size())
	for i in range(box.size()):
		c[i+crypto_secretbox_BOXZEROBYTES] = box[i];
	if (c.size() < 32):
		return null;
	if (crypto_secretbox_open(m, c, c.size(), nonce, key) != 0):
		return null;
	return m.slice(crypto_secretbox_ZEROBYTES);

func box(msg, nonce, publicKey, secretKey):
	var k = box_before(publicKey, secretKey);
	return secretbox(msg, nonce, k);

func box_open(msg, nonce, publicKey, secretKey):
	var k = box_before(publicKey, secretKey);
	return secretbox_open(msg, nonce, k);

func crypto_scalarmult(q, n, p):
	var z = PackedByteArray()
	z.resize(32)
	var x = PackedInt64Array()
	x.resize(80)
	var r
	var a = gf([])
	var b = gf([])
	var c = gf([])
	var d = gf([])
	var e = gf([])
	var f = gf([])
	for i in range(31):
		z[i] = n[i]
	
	z[31] = (n[31] & 127) | 64;
	z[0] &= 248;
	unpack25519(x,p);
	for i in range(16):
		b[i] = x[i];
		d[i] = 0
		a[i] = 0
		c[i] = 0;

	a[0] = 1
	d[0] = 1;
	for i in range(254, -1, -1):
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
		M(a,c,_121665);
		A(a,a,d);
		M(c,c,a);
		M(a,d,f);
		M(d,b,x);
		S(b,e);
		sel25519(a,b,r);
		sel25519(c,d,r);

	for i in range(16):
		x[i+16] = a[i];
		x[i+32] = c[i];
		x[i+48] = b[i];
		x[i+64] = d[i];

	var x32 = x.slice(32);
	var x16 = x.slice(16);
	inv25519(x32, x32);
	M(x16, x16, x32);
	pack25519(q, x16);
	return 0;

func randombytes(x, size):
	for i in range(size):
		x[i] = randi() % 256
		#x[i] = 1

func crypto_box_keypair(y, x):
	randombytes(x, 32);
	return crypto_scalarmult_base(y, x);

func crypto_scalarmult_base(q, n):
	return crypto_scalarmult(q, n, _9);

func box_keypair():
	var pk = PackedByteArray()
	pk.resize(crypto_box_PUBLICKEYBYTES)
	var sk = PackedByteArray()
	sk.resize(crypto_box_SECRETKEYBYTES)
	crypto_box_keypair(pk, sk);
	return [sk, pk];

# Called when the node enters the scene tree for the first time.
func _ready():
	_0.resize(16)
	_9.resize(32)
	_9[0] = 9;
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

}
