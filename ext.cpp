#include "ext.h"
#include "../common/itemcost.h"

const TITEM_RESOURCE_INFO c_tItemResouceInfoTable[GAME_ITEM_COUNT+1] =
{
	{GAME_ITEM_01, GAME_ITEM_01_RECT, GAME_ITEM_01_TEXT, GAME_ITEM_01_COST},
	{GAME_ITEM_02, GAME_ITEM_02_RECT, GAME_ITEM_02_TEXT, GAME_ITEM_02_COST},
	{GAME_ITEM_03, GAME_ITEM_03_RECT, GAME_ITEM_03_TEXT, GAME_ITEM_03_COST},
	{GAME_ITEM_04, GAME_ITEM_04_RECT, GAME_ITEM_04_TEXT, GAME_ITEM_04_COST},
	{GAME_ITEM_05, GAME_ITEM_05_RECT, GAME_ITEM_05_TEXT, GAME_ITEM_05_COST},
	{GAME_ITEM_06, GAME_ITEM_06_RECT, GAME_ITEM_06_TEXT, GAME_ITEM_06_COST},
	{GAME_ITEM_07, GAME_ITEM_07_RECT, GAME_ITEM_07_TEXT, GAME_ITEM_07_COST},
	{GAME_ITEM_08, GAME_ITEM_08_RECT, GAME_ITEM_08_TEXT, GAME_ITEM_08_COST},
	{GAME_ITEM_09, GAME_ITEM_09_RECT, GAME_ITEM_09_TEXT, GAME_ITEM_09_COST},
	{GAME_ITEM_10, GAME_ITEM_10_RECT, GAME_ITEM_10_TEXT, GAME_ITEM_10_COST},
	{GAME_ITEM_11, GAME_ITEM_11_RECT, GAME_ITEM_11_TEXT, GAME_ITEM_11_COST},
	{GAME_ITEM_12, GAME_ITEM_12_RECT, GAME_ITEM_12_TEXT, GAME_ITEM_12_COST},
	{GAME_ITEM_13, GAME_ITEM_13_RECT, GAME_ITEM_13_TEXT, GAME_ITEM_13_COST},
	{GAME_ITEM_14, GAME_ITEM_14_RECT, GAME_ITEM_14_TEXT, GAME_ITEM_14_COST},
	{GAME_ITEM_15, GAME_ITEM_15_RECT, GAME_ITEM_15_TEXT, GAME_ITEM_15_COST},
	{GAME_ITEM_16, GAME_ITEM_16_RECT, GAME_ITEM_16_TEXT, GAME_ITEM_16_COST},
	{GAME_ITEM_17, GAME_ITEM_17_RECT, GAME_ITEM_17_TEXT, GAME_ITEM_17_COST},
	{GAME_ITEM_18, GAME_ITEM_18_RECT, GAME_ITEM_18_TEXT, GAME_ITEM_18_COST},
	{GAME_ITEM_19, GAME_ITEM_19_RECT, GAME_ITEM_19_TEXT, GAME_ITEM_19_COST},
//	{GAME_ITEM_20, GAME_ITEM_20_RECT, GAME_ITEM_20_TEXT, GAME_ITEM_21_COST},
	{0x00000000, 0,0,0,0, L""}
};

HRESULT LoadTextureFromFiler(WCHAR* path, LPDIRECT3DTEXTURE9 *pTex)
{
	HRESULT hr = S_OK;
	BYTE*	pBuf = NULL;
	size_t	nBufferSize;
	if (!g_pFiler->GetFileMemory(L"data/exit.png", &pBuf, &nBufferSize))
		return E_FAIL;
	*pTex = NULL;
	V(D3DXCreateTextureFromFileInMemory(g_pDevice, pBuf, nBufferSize, pTex));
	SafeDeleteArray(pBuf);
	if (hr == E_FAIL)
		SafeRelease(*pTex);
	return hr;
}

HRESULT LoadTextureFromFiler(WCHAR* path, LPDIRECT3DTEXTURE9 *pTex, int *pWidth, int* pHeight)
{
	HRESULT hr = S_OK;
	BYTE*	pBuf = NULL;
	size_t	nBufferSize;
	if (!g_pFiler->GetFileMemory(path, &pBuf, &nBufferSize))
		return E_FAIL;
	D3DXIMAGE_INFO	imginfo;
	*pTex = NULL;
	V(D3DXCreateTextureFromFileInMemoryEx(g_pDevice, pBuf, nBufferSize, 0,0, 1, 0, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0x00000000, &imginfo, NULL, pTex));
	SafeDeleteArray(pBuf);
	if (hr == E_FAIL)
		SafeRelease(*pTex);

	if (pWidth)	*pWidth = imginfo.Width;
	if (pHeight)	*pHeight= imginfo.Height;

	return hr;
}

/* 
   A C-program for MT19937, with initialization improved 2002/2/10.
   Coded by Takuji Nishimura and Makoto Matsumoto.
   This is a faster version by taking Shawn Cokus's optimization,
   Matthe Bellew's simplification, Isaku Wada's real version.

   Before using, initialize the state by using init_genrand(seed) 
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/
/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))

static unsigned long s_rand_state[N]; /* the array for the state vector  */
static int s_rand_left = 1;
static int s_rand_initf = 0;
static unsigned long *next;
/* initializes state[N] with a seed */
void init_genrand(unsigned long s)
{
    int j;
    s_rand_state[0]= s & 0xffffffffUL;
    for (j=1; j<N; j++) {
        s_rand_state[j] = (1812433253UL * (s_rand_state[j-1] ^ (s_rand_state[j-1] >> 30)) + j); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array state[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        s_rand_state[j] &= 0xffffffffUL;  /* for >32 bit machines */
    }
    s_rand_left = 1; s_rand_initf = 1;
}
static void next_state(void)
{
    unsigned long *p=s_rand_state;
    int j;

    /* if init_genrand() has not been called, */
    /* a default initial seed is used         */
    if (s_rand_initf==0) init_genrand(5489UL);

    s_rand_left = N;
    next = s_rand_state;
    
    for (j=N-M+1; --j; p++) 
        *p = p[M] ^ TWIST(p[0], p[1]);

    for (j=M; --j; p++) 
        *p = p[M-N] ^ TWIST(p[0], p[1]);

    *p = p[M-N] ^ TWIST(p[0], s_rand_state[0]);
}
/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;

    if (--s_rand_left == 0) next_state();
    y = *next++;

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}