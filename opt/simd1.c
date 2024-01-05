 void simd_v1_combine(vec_ptr v, data_t *dest)
{
	long i;
	vec_t accum0,accum1,accum2,accum3;
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
	}

	/* 4 * VSIZE x 4 * VSIZE unrolling */
	while (cnt >= 4*VSIZE) {
		vec_t chunk0 = *((vec_t *) data);
		vec_t chunk1 = *((vec_t *) (data+VSIZE));
		vec_t chunk2 = *((vec_t *) (data+2*VSIZE));
		vec_t chunk3 = *((vec_t *) (data+3*VSIZE));
		accum0 = accum0 OP chunk0;
		accum1 = accum1 OP chunk1;
		accum2 = accum2 OP chunk2;
		accum3 = accum3 OP chunk3;
		data += 4*VSIZE;
		cnt -= 4*VSIZE;
	}


	/* Single-step through remaining elements */
	while (cnt) {
		result = result OP *data++;
		cnt--;
	}

	/* Combine into single accumulator */
	accum0 = (accum0 OP accum1) OP (accum2 OP accum3);
	/* Combine results from accumulators within vector */
	for (i = 0; i < VSIZE; i++)
		result = result OP accum0[i];


	/* Store result */
	*dest = result;
}