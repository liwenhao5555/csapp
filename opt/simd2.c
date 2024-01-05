 void simd_v1_combine(vec_ptr v, data_t *dest)
{
	long i;
	vec_t accum;
	data_t *data = get_vec_start(v);
	int cnt = vec_length(v);
	data_t result = IDENT;

	/* Initialize all accum entries to IDENT */
	for (i = 0; i < VSIZE; i++)
		accum[i] = IDENT;

	/* Single step until have memory alignment */
	while ((((size_t) data) % VBYTES) != 0 && cnt) {
		result = result OP *data++;
		cnt--;
	}yt

	/* Step through data with VSIZE-way parallelism */
	while (cnt >= 4*VSIZE) {
		vec_t chunk0 = *((vec_t *) data);
		vec_t chunk1 = *((vec_t *) (data+VSIZE));
		vec_t chunk2 = *((vec_t *) (data+2*VSIZE));
		vec_t chunk3 = *((vec_t *) (data+3*VSIZE));
		accum = accum OP
			((chunk0 OP chunk1) OP (chunk2 OP chunk3));
		data += 4*VSIZE;
		cnt -= 4*VSIZE;	
	}

	/* Single-step through remaining elements */
	while (cnt) {
		result = result OP *data++;
		cnt--;
	}

	/* Combine elements of accumulator vector */
	for (i = 0; i < VSIZE; i++)
		result = result OP accum[i];

	/* Store result */
	*dest = result;
}