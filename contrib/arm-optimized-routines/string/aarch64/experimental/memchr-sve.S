/*
 * memchr - find a character in a memory zone
 *
 * Copyright (c) 2018-2022, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

#include "asmdefs.h"

.arch armv8-a+sve

/* Assumptions:
 *
 * ARMv8-a, AArch64
 * SVE Available.
 */

ENTRY (__memchr_aarch64_sve)
	dup	z1.b, w1			/* duplicate c to a vector */
	setffr					/* initialize FFR */
	mov	x3, 0				/* initialize off */

	.p2align 4
0:	whilelo	p1.b, x3, x2			/* make sure off < max */
	b.none	9f

	/* Read a vector's worth of bytes, bounded by max,
	   stopping on first fault.  */
	ldff1b	z0.b, p1/z, [x0, x3]
	rdffrs	p0.b, p1/z
	b.nlast	2f

	/* First fault did not fail: the vector bounded by max is valid.
	   Avoid depending on the contents of FFR beyond the branch.  */
	incb	x3				/* speculate increment */
	cmpeq	p2.b, p1/z, z0.b, z1.b		/* search for c */
	b.none	0b
	decb	x3				/* undo speculate */

	/* Found C.  */
1:	brkb	p2.b, p1/z, p2.b	/* find the first c */
	add	x0, x0, x3		/* form partial pointer */
	incp	x0, p2.b		/* form final pointer to c */
	ret

	/* First fault failed: only some of the vector is valid.
	   Perform the comparision only on the valid bytes.  */
2:	cmpeq	p2.b, p0/z, z0.b, z1.b
	b.any	1b

	/* No C found.  Re-init FFR, increment, and loop.  */
	setffr
	incp	x3, p0.b
	b	0b

	/* Found end of count.  */
9:	mov	x0, 0			/* return null */
	ret

END (__memchr_aarch64_sve)
